#include "fow/Editor/EditorInstance.hpp"
#include "fow/Editor/ImGui.hpp"

namespace fow {
    static bool s_running = false;

    void EditorInit() {
        s_running = true;
    }
    void EditorTerminate() {
        s_running = false;
    }

    void EditorTick() {
        if (!s_running) return;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New project..")) {
                    // TODO: Open new project popup
                }
                if (ImGui::MenuItem("Open project..")) {
                    // TODO: Open project browser
                }
                if (ImGui::MenuItem("Generate Random Map")) { // TODO: Add compiler flag to enable/disable RMG
                    // TODO: Open RMG dialog
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) {
                    EditorTerminate();
                    // TODO: Go back to main menu
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    bool EditorIsRunning() {
        return s_running;
    }
}
