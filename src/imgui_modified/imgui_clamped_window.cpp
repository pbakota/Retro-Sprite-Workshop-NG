#include <string>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_clamped_window.h"

void ImGui::ClampingWindowToTheScreenEdges(const char *title) {
    ImGuiWindow *existingWindow = ImGui::FindWindowByName(title);
    if (existingWindow != nullptr) {
        bool needsClampToScreen = false;
        ImVec2 targetPos = existingWindow->Pos;
        if (existingWindow->Pos.x < 0.0f) {
            needsClampToScreen = true;
            targetPos.x = 0.0f;
        }
        else if (existingWindow->Size.x + existingWindow->Pos.x > ImGui::GetMainViewport()->Size.x) {
            needsClampToScreen = true;
            targetPos.x = ImGui::GetMainViewport()->Size.x - existingWindow->Size.x;
        }
        if (existingWindow->Pos.y < 0.0f) {
            needsClampToScreen = true;
            targetPos.y = 0.0f;
        }
        else if (existingWindow->Size.y + existingWindow->Pos.y > ImGui::GetMainViewport()->Size.y) {
            needsClampToScreen = true;
            targetPos.y = ImGui::GetMainViewport()->Size.y - existingWindow->Size.y;
        }

        if (needsClampToScreen) { // Necessary to prevent window from constantly undocking itself if docked.
            ImGui::SetNextWindowPos(targetPos, ImGuiCond_Always);
        }
    }
}
