#pragma once
#include "imgui.h"

struct StatusBar {
    int zoomIndex = 3;
    const char* zoomValues[8] = {
        "25%",
        "50%",
        "75%",
        "100%",
        "125%",
        "150%",
        "175%",
        "200%",
    };
    int col, row;
    bool is_rowcol_visible = false;
    bool is_zoom_visible = false;
    size_t totalBytes = 0;
    size_t spriteCount = 0;

    StatusBar() {}

    void render() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f,3.0f));
        ImGui::SetNextWindowSize(ImVec2(-1, 50.0f));
        ImGui::Begin("Status bar", nullptr, ImGuiWindowFlags_NoScrollbar);
        if(ImGui::BeginTable("#statusBar",3, ImGuiTableFlags_NoBordersInBody)) {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 200.0f);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 300.0f);
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::AlignTextToFramePadding(); ImGui::Text("%lu images, %lu bytes", spriteCount, totalBytes);
                ImGui::TableNextColumn(); if(is_rowcol_visible) { ImGui::AlignTextToFramePadding(); ImGui::Text("Col=%d, Row=%d, ByteIndex=%d", col, row, 55); }

                ImGui::TableNextColumn(); if(is_zoom_visible) {
                    auto right_pos = ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - 270.0f; ImGui::SetCursorPosX(right_pos);
                    ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted("Zoom:");
                    ImGui::SameLine(); ImGui::PushID(0); ImGui::SetNextItemWidth(200.0f); if(ImGui::BeginCombo("", zoomValues[zoomIndex])) {
                        for (int n = 0; n < IM_ARRAYSIZE(zoomValues); n++) {
                            const bool is_selected = (zoomIndex == n);
                            if (ImGui::Selectable(zoomValues[n], is_selected)) {
                                zoomIndex = n;
                            }

                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    } ImGui::PopID();
                }
            }
            ImGui::EndTable();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
};
