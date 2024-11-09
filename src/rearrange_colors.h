#pragma once
#include <string>
#include "imgui.h"
#include "sprite_manager.h"

struct RearrangeColors
{
    RearrangeColors() {}

    const char* colorValues[4] = {
        "Background Color Pixels (00)",
        "Multi1 Color Pixels (01)",
        "Multi2 Color Pixels (10)",
        "Character Color Pixels (11)",
    };

    bool show(bool multicolor, size_t selected[], bool *open) {
        if(!*open)
            return false;

        ImGuiStyle& style = ImGui::GetStyle();

        bool completed = false;
        std::string title = vformat("Rearrange Color Pixels (%d colors mode)", multicolor ? 4: 2);

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::SetNextWindowSize(ImVec2(640.0f,180.0f));
        ImGui::OpenPopup(title.c_str(), ImGuiPopupFlags_None);
        if(ImGui::BeginPopupModal(title.c_str(), open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            if (ImGui::BeginTable("#colors", 3, ImGuiTableFlags_NoBordersInBody))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 210.0f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

                for(size_t i=0;i<IM_ARRAYSIZE(colorValues);++i) {
                    if(!multicolor && !(i==0 || i == 3)) continue;
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::TextUnformatted(colorValues[i]);
                    ImGui::TableNextColumn(); ImGui::TextUnformatted(selected[i] == i ? "remains unchanged" : "will change to");
                    ImGui::TableNextColumn(); ImGui::PushID(i); ImGui::SetNextItemWidth(-1); if(ImGui::BeginCombo("", colorValues[selected[i]])) {
                        for (size_t n = 0; n < IM_ARRAYSIZE(colorValues); ++n) {
                            if(!multicolor && !(n==0 || n==3)) continue;
                            const bool is_selected = selected[i] == n;
                            if (ImGui::Selectable(colorValues[n], is_selected)) {
                                selected[i] = n;
                            }

                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    } ImGui::PopID();
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TableNextColumn();
                ImGui::TableNextColumn();
                ImGui::Dummy(ImVec2(0, 20.0f));
                ImVec2 buttonSize(100,20);
                float widthNeeded = buttonSize.x + style.ItemSpacing.x + buttonSize.x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - widthNeeded);
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30.0f);
                ImGui::PushID(5); if(ImGui::Button("OK", buttonSize)) {
                    completed = true;
                    *open = false;
                } ImGui::PopID(); ImGui::SameLine();
                ImGui::PushID(6); if(ImGui::Button("Cancel", buttonSize)) {
                    *open = false;
                } ImGui::PopID();
                ImGui::EndTable();
            }
            ImGui::EndPopup();
        }

        return completed;
    }
};