#ifndef FOW_ANY_HPP

#include <fow/Shared.hpp>

#include <typeindex>

namespace fow {
    class FOW_SHARED_API Any {
        std::type_index m_type;
        void* m_pData;
    public:
        Any() : m_type(typeid(void)), m_pData(nullptr) { }

        template<typename T>
        [[nodiscard]] constexpr bool is_type_of() const { return typeid(T) == m_type; }
        constexpr const std::type_index& type() const { return m_type; }

        template<typename T>
        constexpr const T* cast_to() const { }
    };
}