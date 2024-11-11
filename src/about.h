#pragma once
#include "imgui.h"

struct About {
    const char* title = "About";

    void show(bool *open) {
        if(!*open) return;

        ImGuiStyle& style = ImGui::GetStyle();

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::SetNextWindowSize(ImVec2(320.0f,100.0f));
        ImGui::OpenPopup(title, ImGuiPopupFlags_None);
        if(ImGui::BeginPopupModal(title, open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            ImGui::TextUnformatted("Retro Sprite Workshop Clone");
            ImGui::TextUnformatted("(c)2024");
            ImGui::Dummy(ImVec2(0, 20.0f));
            ImVec2 buttonSize(100,20);
            float widthNeeded = buttonSize.x + style.ItemSpacing.x + buttonSize.x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - widthNeeded);
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30.0f);
            ImGui::PushID(5); if(ImGui::Button("OK", buttonSize)) {
                *open = false;
            } ImGui::PopID(); ImGui::SameLine();
            ImGui::EndPopup();
        }
    }
};

