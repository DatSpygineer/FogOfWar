#include <fstream>

#include "fow/Shared.hpp"
#include "fow/Engine.hpp"

#include <imgui.h>
#include <ranges>

#include "imgui_internal.h"

namespace fow {
    namespace Console {
        static bool s_cheats_enabled  = false;
        static bool s_console_visible = false;
        static Vector<String> s_console_textbuf;
        static char s_console_input[512];

        static void ConsoleMessageCallback(const LogLevel level, const Time::TimePoint& time_point, const String& message, const std::source_location& location) {
            s_console_textbuf.emplace_back(std::format("[{}] {}", level, message));
        }

        Result<> Initialize() {
            const auto cfg_path = Engine::GetGameBasePath() / "cfg";
            if (const auto user_settings_path = cfg_path / "user.cfg"; user_settings_path.exists()) {
                if (const auto result = ExecuteConfig(user_settings_path); !result.has_value()) {
                    return result;
                }
            }

            Debug::SetMessageSentCallback(&ConsoleMessageCallback);

            return Success();
        }
        void Terminate() {
            const auto cfg_path = Engine::GetGameBasePath() / "cfg";
            if (!cfg_path.exists()) {
                Path::CreateDirectory(cfg_path, true);
            }

            const auto& cvars = CVar::GetAll();
            std::ofstream ofs((cfg_path / "user.cfg").as_std_path());
            ofs << "# This is auto-generated! Do not modify!" << std::endl;
            ofs << std::endl;
            for (const auto& [ name, cvar ] : cvars) {
                if (cvar->flags() & CVarFlags::SaveToConfig && !cvar->is_callable()) {
                    const auto value = cvar->as_string()->as_std_str();
                    const auto quotes_required = value.find_first_of(" \t\n\r") != -1;
                    ofs << name.as_std_str() << " ";
                    if (quotes_required) {
                        ofs << "\"";
                    }
                    ofs << value;
                    if (quotes_required) {
                        ofs << "\"";
                    }
                    ofs << std::endl;
                }
            }
            ofs << std::endl;
            ofs.close();
        }
        Result<> ExecuteCommand(const String& command) {
            Vector<String> args;
            String temp;
            char q = 0;
            for (size_t i = 0; i < command.size(); ++i) {
                const char c = command[i];
                if (temp.is_empty() && q == 0) {
                    if (std::isspace(c)) {
                        continue;
                    }
                    if (c == '#') {
                        break;
                    }
                    if (c == '\'' || c == '"') {
                        q = c;
                    } else {
                        temp.append(c);
                    }
                } else {
                    if (q == 0) {
                        if (std::isspace(c)) {
                            args.push_back(temp);
                            temp.clear();
                        } else if (c == ';') {
                            temp.clear();
                            args.push_back(temp);
                            if (const auto result = ExecuteCommand(args); !result.has_value()) {
                                return result;
                            }
                            args.clear();
                        } else {
                            temp.append(c);
                        }
                    } else {
                        if (c == q) {
                            args.push_back(temp);
                            temp.clear();
                            q = 0;
                        } else {
                            temp.append(c);
                        }
                    }
                }
            }

            if (!temp.is_empty()) {
                args.push_back(temp);
            }

            if (!args.empty()) {
                if (const auto result = ExecuteCommand(args); !result.has_value()) {
                    return result;
                }
            }
            return Success();
        }
        Result<> ExecuteCommand(const Vector<String>& args) {
            std::ostringstream cmd_string;
            for (const auto& arg : args) {
                if (cmd_string.tellp() != 0) {
                    cmd_string << " ";
                }
                if (arg.find_any_first_of(" \t\n\r") != -1) {
                    cmd_string << std::format("'{}'", arg);
                } else {
                    cmd_string << arg;
                }
            }

            if (args.empty()) {
                return Failure(std::format("Failed to execute command \"{}\": No arguments specified!", cmd_string.str()));
            }

            const auto& name = args.at(0);
            const auto& args_remain = Collections::CopySlice(args, 1);
            const auto& cvar = CVar::Get(name);
            if (cvar.has_value()) {
                if (cvar.value()->is_callable()) {
                    return cvar.value()->call(args_remain);
                }
                if (args_remain.size() > 1) {
                    Debug::LogWarning(std::format("CVar \"{}\" takes a single value! Extra values ignored...", name));
                }
                if (args_remain.empty()) {
                    Debug::LogInfo(std::format("{} = {}", name, cvar.value()->value()));
                    return Success();
                }
                return cvar.value()->set(args_remain.at(0));
            }
            return Failure(std::format("Failed to execute command \"{}\": Unknown cvar \"{}\"", cmd_string.str(), name));
        }
        void SetConsoleVisible(const bool visible) {
            s_console_visible = visible;
        }
        bool IsConsoleVisible() {
            return s_console_visible;
        }
        void ToggleConsoleVisible() {
            s_console_visible = !s_console_visible;
        }
        Result<> ExecuteConfig(const String& config_name) {
            return ExecuteConfig(Engine::GetGameBasePath() / "cfg" / config_name);
        }
        Result<> ExecuteConfig(const Path& path) {
            std::ifstream ifs(path.as_std_path());
            if (!ifs.is_open()) {
                return Failure(std::format("Failed to open configuration file \"{}\": File doesn't exists", path));
            }

            std::string line;
            while (std::getline(ifs, line)) {
                if (String(line).trim_begin().starts_with('#')) {
                    continue;
                }
                if (const auto& result = ExecuteCommand(line); !result.has_value()) {
                    return result;
                }
            }
            return Success();
        }

        void UpdateConsole() {
            if (!s_console_visible) {
                return;
            }
            auto io = ImGui::GetIO();

            if (ImGui::Begin("Developer Console", &s_console_visible)) {
                if (ImGui::BeginChild("Output", ImVec2 { 0, -24 }, ImGuiChildFlags_Borders)) {
                    for (auto text : s_console_textbuf) {
                        bool style_pushed = true;
                        if (text.starts_with("[DEBUG]")) {
                            text.remove_prefix("[DEBUG]");
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
                        } else if (text.starts_with("[INFO]")) {
                            text.remove_prefix("[INFO]");
                            style_pushed = false;
                        } else if (text.starts_with("[WARNING]")) {
                            text.remove_prefix("[WARNING]");
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                        } else if (text.starts_with("[ERROR]")) {
                            text.remove_prefix("[ERROR]");
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.125f, 0.125f, 1.0f));
                        } else if (text.starts_with("[FATAL]")) {
                            text.remove_prefix("[FATAL]");
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));
                        } else {
                            style_pushed = false;
                        }
                        text.trim();
                        ImGui::Text("%s", text.as_cstr());
                        if (style_pushed) ImGui::PopStyleColor();
                    }
                    ImGui::EndChild();
                }
                if (ImGui::BeginTable("##InputTable", 2)) {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 64);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    ImGui::InputTextWithHint("##Command", "Type in a command here", s_console_input, 512);

                    const auto input_focused = ImGui::IsItemFocused();
                    if (input_focused) {
                        ImGui::OpenPopup("##CommandHelpPopup");
                    } else {
                        ImGui::CloseCurrentPopup();
                    }

                    Vector<String> searches = { };
                    for (const auto& cvar : CVar::GetAvailableNames()) {
                        if (strlen(s_console_input) == 0 || cvar.starts_with(s_console_input, StringCompareType::CaseInsensitive)) {
                            searches.emplace_back(cvar);
                        }
                    }
                    std::sort(searches.begin(), searches.end(), [](const auto& a, const auto& b) { return a < b; });

                    const auto pos  = ImGui::GetItemRectMin();
                    const auto size = ImGui::GetItemRectSize();
                    ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + size.y));
                    ImGui::SetNextWindowSize(ImVec2(size.x, 0));
                    if (ImGui::BeginPopup("##CommandHelpPopup", ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
                        for (const auto cvar : searches) {
                            if (ImGui::Selectable(cvar.as_cstr())) {
                                strncpy(s_console_input, cvar.as_cstr(), cvar.size());
                                s_console_input[cvar.size()] = 0;
                                ImGui::CloseCurrentPopup();
                            }
                        }
                        ImGui::EndPopup();
                    }

                    ImGui::TableNextColumn();
                    if (ImGui::Button("Submit") || (input_focused && ImGui::IsKeyPressed(ImGuiKey_Enter))) {
                        if (s_console_input[0] != 0) {
                            Debug::Assert(ExecuteCommand(s_console_input));
                        }
                        memset(s_console_input, 0, 512);
                    }
                    ImGui::EndTable();
                }
                ImGui::End();
            }
        }
    }

    HashMap<String, SharedPtr<CVar>> CVar::s_registry = { };

    Result<> CVar::call(const Vector<String>& args) const {
        if (!is_callable()) {
            return Failure(std::format("Failed to call cvar \"{}\": CVar is not callable!", m_sName));
        }

        if (m_eFlags & CVarFlags::IsCheat && !Console::s_cheats_enabled) {
            return Failure(std::format("CVar \"{}\" requires cheats to be enabled!", m_sName));
        }
        return std::get<ConsoleCommand>(m_vValue)(args);
    }

    Result<> CVar::set(const CVarValue& value) {
        if (is_callable()) {
            return Failure(std::format("Failed to set cvar \"{}\": CVar is a command!", m_sName));
        }

        if (m_eFlags & CVarFlags::IsCheat && !Console::s_cheats_enabled) {
            return Failure(std::format("CVar \"{}\" requires cheats to be enabled!", m_sName));
        }
        m_vValue = value;

        if (m_fnSetCallback != nullptr) {
            m_fnSetCallback(shared_from_this());
        }

        Debug::LogDebug(std::format("CVar \"{}\" is set to value \"{}\"", m_sName, m_vValue));
        return Success();
    }

    Vector<String> CVar::GetAvailableNames() {
        Vector<String> result;
        for (const auto& key: s_registry | std::views::keys) {
            result.emplace_back(key);
        }
        return result;
    }
}
