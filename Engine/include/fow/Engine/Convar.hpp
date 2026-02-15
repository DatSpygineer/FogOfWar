#ifndef FOW_CONVAR_HPP
#define FOW_CONVAR_HPP

#include <variant>

#include "fow/Shared.hpp"
#include "rfl/enums.hpp"
#include "rfl/internal/variant/result_t.hpp"

namespace fow {
    namespace CVarFlags {
        enum Type {
            Default          = 0b0000'0000,
            IsCheat          = 0b0000'0001,
            SinglePlayerOnly = 0b0000'0010,
            MultiPlayerOnly  = 0b0000'0100,
            UserSettings     = 0b0000'1000,
            SaveToConfig     = 0b0001'0000
        };

        FOW_CONSTEXPR Type operator| (const Type a, const Type b) {
            return static_cast<Type>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }
        FOW_CONSTEXPR bool operator& (const Type a, const Type b) {
            return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != 0;
        }
    }

    class CVar;
    using CVarPtr = std::shared_ptr<CVar>;
    using ConsoleCommand  = std::function<Result<>(const Vector<String>&)>;
    using CVarSetCallback = std::function<void(const CVarPtr&)>;

    using CVarValue = std::variant<bool, int, float, String, glm::vec2, glm::vec3, glm::vec4, ConsoleCommand>;
    enum class CVarValueType {
        Bool,
        Int,
        Float,
        String,
        Vec2,
        Vec3,
        Vec4,
        Callable
    };

    namespace Console {
        FOW_ENGINE_API Result<> Initialize();
        FOW_ENGINE_API void Terminate();
        FOW_ENGINE_API Result<> ExecuteCommand(const String& command);
        FOW_ENGINE_API Result<> ExecuteCommand(const Vector<String>& args);
        FOW_ENGINE_API Result<> ExecuteConfig(const String& config_name);
        FOW_ENGINE_API Result<> ExecuteConfig(const Path& path);

        FOW_ENGINE_API void SetConsoleVisible(bool visible);
        FOW_ENGINE_API bool IsConsoleVisible();
        FOW_ENGINE_API void ToggleConsoleVisible();

        FOW_ENGINE_API void UpdateConsole();
    }

    class FOW_ENGINE_API CVarRedefinitionException : public std::exception {
        String m_sMessage;
    public:
        explicit CVarRedefinitionException(const String& name) noexcept: m_sMessage(std::format("Duplicate definition of cvar \"{}\"", name)) { }

        FOW_CONSTEXPR const char* what() const noexcept override {
            return m_sMessage.as_cstr();
        }
    };

    class FOW_ENGINE_API CVar final : public std::enable_shared_from_this<CVar> {
        String m_sName;
        CVarValue m_vValue;
        CVarFlags::Type m_eFlags;
        CVarSetCallback m_fnSetCallback;

        static HashMap<String, SharedPtr<CVar>> s_registry;
#if __cplusplus >= 202302L
        FOW_CONSTEXPR CVar(const String& name, const CVarValue& value, const CVarFlags::Type flags, const CVarSetCallback& callback) :
            m_sName(name), m_vValue(value), m_eFlags(flags), m_fnSetCallback(callback) { }
#else
        CVar(const String& name, const CVarValue& value, const CVarFlags::Type flags, const CVarSetCallback& callback) :
            m_sName(name), m_vValue(value), m_eFlags(flags), m_fnSetCallback(callback) { }
#endif
    public:
        FOW_CONSTEXPR CVar(const CVar&) = delete;
#if __cplusplus >= 202302L
        FOW_CONSTEXPR CVar(CVar&& other) noexcept : m_sName(std::move(other.m_sName)), m_vValue(std::move(other.m_vValue)), m_eFlags(std::move(other.m_eFlags)), m_fnSetCallback(std::move(other.m_fnSetCallback)) { }
#else
        CVar(CVar&& other) noexcept : m_sName(std::move(other.m_sName)), m_vValue(std::move(other.m_vValue)), m_eFlags(std::move(other.m_eFlags)), m_fnSetCallback(std::move(other.m_fnSetCallback)) { }
#endif
        CVar& operator=(const CVar&) = delete;
        CVar& operator=(CVar&&) noexcept = default;

        FOW_CONSTEXPR CVarValueType value_type() const { return static_cast<CVarValueType>(m_vValue.index()); }
        FOW_CONSTEXPR Result<bool> as_bool() const {
            if (value_type() == CVarValueType::Bool) {
                return std::get<bool>(m_vValue);
            }
            if (value_type() == CVarValueType::Int) {
                return std::get<int>(m_vValue) != 0;
            }
            if (value_type() == CVarValueType::Float) {
                return std::get<float>(m_vValue) != 0.0f;
            }
            if (value_type() == CVarValueType::String) {
                return StringToBool(std::get<String>(m_vValue));
            }
            return Failure(std::format("Cannot convert type \"{}\" to boolean", rfl::enum_to_string<CVarValueType>(value_type())));
        }
        FOW_CONSTEXPR Result<int> as_int() const {
            if (value_type() == CVarValueType::Bool) {
                return std::get<bool>(m_vValue) ? 1 : 0;
            }
            if (value_type() == CVarValueType::Int) {
                return std::get<int>(m_vValue);
            }
            if (value_type() == CVarValueType::Float) {
                return static_cast<int>(std::get<float>(m_vValue));
            }
            if (value_type() == CVarValueType::String) {
                return StringToInt<int>(std::get<String>(m_vValue));
            }
            return Failure(std::format("Cannot convert type \"{}\" to integer", rfl::enum_to_string<CVarValueType>(value_type())));
        }
        FOW_CONSTEXPR Result<float> as_float() const {
            if (value_type() == CVarValueType::Bool) {
                return std::get<bool>(m_vValue) ? 1.0f : 0.0f;
            }
            if (value_type() == CVarValueType::Int) {
                return std::get<int>(m_vValue);
            }
            if (value_type() == CVarValueType::Float) {
                return std::get<float>(m_vValue);
            }
            if (value_type() == CVarValueType::String) {
                return StringToFloat<float>(std::get<String>(m_vValue));
            }
            return Failure(std::format("Cannot convert type \"{}\" to float", rfl::enum_to_string<CVarValueType>(value_type())));
        }
#if __cplusplus >= 202302L
        FOW_CONSTEXPR Result<String> as_string() const {
#else
        inline Result<String> as_string() const {
#endif
            if (value_type() == CVarValueType::Bool) {
                return String(std::get<0>(m_vValue) ? "true" : "false");
            }
            if (value_type() == CVarValueType::Int) {
                return String(std::to_string(std::get<1>(m_vValue)));
            }
            if (value_type() == CVarValueType::Float) {
                return String(std::to_string(std::get<2>(m_vValue)));
            }
            if (value_type() == CVarValueType::String) {
                return std::get<3>(m_vValue);
            }
            if (value_type() == CVarValueType::Vec2) {
                return String(std::format("{}", std::get<4>(m_vValue)));
            }
            if (value_type() == CVarValueType::Vec3) {
                return String(std::format("{}", std::get<5>(m_vValue)));
            }
            if (value_type() == CVarValueType::Vec4) {
                return String(std::format("{}", std::get<6>(m_vValue)));
            }
            return Failure(std::format("Cannot convert type \"{}\" to string", rfl::enum_to_string<CVarValueType>(value_type())));
        }
        FOW_CONSTEXPR Result<glm::vec2> as_vec2() const {
            if (value_type() == CVarValueType::Vec2) {
                return std::get<glm::vec2>(m_vValue);
            }
            if (value_type() == CVarValueType::String) {
                return StringToVec2(std::get<String>(m_vValue));
            }
            return Failure(std::format("Cannot convert type \"{}\" to vec2", rfl::enum_to_string<CVarValueType>(value_type())));
        }
        FOW_CONSTEXPR Result<glm::vec3> as_vec3() const {
            if (value_type() == CVarValueType::Vec3) {
                return std::get<glm::vec3>(m_vValue);
            }
            if (value_type() == CVarValueType::String) {
                return StringToVec3(std::get<String>(m_vValue));
            }
            return Failure(std::format("Cannot convert type \"{}\" to vec3", rfl::enum_to_string<CVarValueType>(value_type())));
        }
        FOW_CONSTEXPR Result<glm::vec<4, float>> as_vec4() const {
            if (value_type() == CVarValueType::Vec4) {
                return std::get<glm::vec4>(m_vValue);
            }
            if (value_type() == CVarValueType::String) {
                return StringToVec4(std::get<String>(m_vValue));
            }
            return Failure(std::format("Cannot convert type \"{}\" to vec4", rfl::enum_to_string<CVarValueType>(value_type())));
        }
        FOW_CONSTEXPR const CVarValue& value() const {
            return m_vValue;
        }
        FOW_CONSTEXPR const String& name() const {
            return m_sName;
        }
        FOW_CONSTEXPR CVarFlags::Type flags() const {
            return m_eFlags;
        }
        FOW_CONSTEXPR bool is_callable() const {
            return value_type() == CVarValueType::Callable;
        }
        Result<> call(const Vector<String>& args) const;
        Result<> set(const CVarValue& value);

#if __cplusplus >= 202302L
        static FOW_CONSTEXPR CVarPtr Create(const String& name, const CVarValue& value, const CVarFlags::Type flags, const CVarSetCallback& callback = nullptr) {
#else
        inline static CVarPtr Create(const String& name, const CVarValue& value, const CVarFlags::Type flags, const CVarSetCallback& callback = nullptr) {
#endif
            if (s_registry.contains(name)) {
                throw CVarRedefinitionException(name);
            }
            s_registry.emplace(name, std::make_shared<CVar>(std::move(CVar { name, value, flags, callback })));
            return s_registry.at(name);
        }
        static FOW_CONSTEXPR Result<CVarPtr> CreateOptional(const String& name, const CVarValue& value, const CVarFlags::Type flags, const CVarSetCallback& callback = nullptr) {
            if (s_registry.contains(name)) {
                return Failure(std::format("CVar \"{}\" already defined!", name));
            }
            s_registry.emplace(name, std::make_shared<CVar>(std::move(CVar { name, value, flags, callback })));
            return s_registry.at(name);
        }
#if __cplusplus >= 202302L
        static FOW_CONSTEXPR CVarPtr CreateOrGet(const String& name, const CVarValue& value, const CVarFlags::Type flags, const CVarSetCallback& callback = nullptr) {
#else
        inline static CVarPtr CreateOrGet(const String& name, const CVarValue& value, const CVarFlags::Type flags, const CVarSetCallback& callback = nullptr) {
#endif
            if (!s_registry.contains(name)) {
                s_registry.emplace(name, std::make_shared<CVar>(std::move(CVar { name, value, flags, callback })));
            }
            return s_registry.at(name);
        }
        static FOW_CONSTEXPR Result<CVarPtr> Get(const String& name) {
            if (s_registry.contains(name)) {
                return Success<CVarPtr>(s_registry.at(name));
            }
            return Failure(std::format("Could not find cvar \"{}\"", name));
        }
        static FOW_CONSTEXPR bool Exists(const String& name) {
            return s_registry.contains(name);
        }
        static FOW_CONSTEXPR const HashMap<String, CVarPtr>& GetAll() {
            return s_registry;
        }
        static Vector<String> GetAvailableNames();
    };
}

#define FOW_DECL_CONSOLE_COMMAND(__name, __flags) \
    inline fow::Result<> __name##_callable(const fow::Vector<fow::String>& args);\
    static fow::CVarPtr  __name = fow::CVar::Create(#__name, { &__name##_callable }, __flags);\
    inline fow::Result<> __name##_callable(const fow::Vector<fow::String>& args)

template<>
struct std::formatter<fow::CVarValue> : std::formatter<std::string> {
    inline auto format(const fow::CVarValue& value, std::format_context& ctx) const {
        std::string result;
        switch (value.index()) {
            case 0: result = std::get<bool>(value) ? "true" : "false"; break;
            case 1: result = std::to_string(std::get<int>(value)); break;
            case 2: result = std::to_string(std::get<float>(value)); break;
            case 3: result = std::get<fow::String>(value).as_std_str(); break;
            case 4: result = std::format("{}", std::get<glm::vec2>(value)); break;
            case 5: result = std::format("{}", std::get<glm::vec3>(value)); break;
            case 6: result = std::format("{}", std::get<glm::vec4>(value)); break;
            default: result = "<callable>"; break;
        }
        return std::formatter<std::string>::format(result, ctx);
    }
};

#endif