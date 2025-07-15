#pragma once

#include "imgui.h"
#include "sprite.h"


struct Animation {

    int currentFrameIndex = 0;
    const int one = 1; // Used for input scalar step
    const float scale = 0.8f; // Scale down the display area slightly

    Animation() {}

    void render(SDL_Renderer *render, Sprite *sp) {
        ImGui::SetNextWindowSize(ImVec2(320.0f, 240.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("Animation View");
        if (ImGui::BeginTabBar("#spriteAnimation", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Animation")) {

                ImVec2 display_size;
                CalculateDisplaySize(sp, display_size);

                if (ImGui::BeginTable("#spriteMetadata", 2, ImGuiTableFlags_NoBordersInBody)) {
                    ImGui::TableSetupColumn("Frames", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, display_size.x);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    DrawFrames(sp, display_size);
                    DrawCursor(sp, display_size);
                    ImGui::TableSetColumnIndex(1);
                    DrawPreview(sp, display_size);
                    ImGui::EndTable();
                }                
                ImGui::EndTabItem();
            }            
            ImGui::EndTabBar();
        }
        ImGui::End();
    }

    void DrawFrames(Sprite *sp, const ImVec2 &display_size) {
        if(ImGui::BeginChild("Frames", ImVec2(0,0), 0, ImGuiWindowFlags_HorizontalScrollbar)) {

            ImVec2 s = ImGui::GetCursorScreenPos();
            ImVec2 p = ImGui::GetCursorPos();

            for(int i=0; i<14; ++i) {
                DrawFrame(sp, s, i+1, display_size);
                s.x += display_size.x + 5.0f; // Add some spacing between frames
            }
            ImGui::SetCursorPosX(p.x + (display_size.x + 5.0f)*14);
            ImGui::EndChild();
        }
    }

    void DrawPreview(Sprite *sp, const ImVec2 &display_size) {
        ImGui::PushID(0); ImGui::SetNextItemWidth(80.0f); if(ImGui::InputScalar("", ImGuiDataType_U8, &sp->animationFPS, &one, nullptr, "%d", 0)) {
            if(sp->animationFPS < 1) {
                sp->animationFPS = 1; // Ensure FPS is at least 1
            }
            if(sp->animationFPS > 50) {
                sp->animationFPS = 50; // Cap FPS at 60
            }
        } ImGui::PopID();
        ImGui::SameLine(); ImGui::Text("FPS");

        // Display the current frame
        // Sprite::Frame &currentFrame = sp->animationFrames[currentFrameIndex];
        //ImGui::Image((ImTextureID)currentFrame.cache, display_size);

        ImVec2 s = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        ImVec2 rect = ImVec2(s.x + display_size.x*scale, s.y + display_size.y*scale);
        dl->AddRectFilled(s, rect, sp->backgroundColor);
    }

    void DrawFrame(Sprite *sp, const ImVec2 &pos, int index, const ImVec2 &display_size) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        // TODO: Replace with actual sprite rendering
        // For now, just draw a rectangle to represent the frame
        dl->AddRectFilled(pos, ImVec2(pos.x + display_size.x, pos.y + display_size.y), sp->backgroundColor);
        dl->AddText(ImVec2(pos.x+2.0f, pos.y+2.0f), IM_COL32(255, 255, 255, 255), std::to_string(index).c_str());
    }
    
    void DrawCursor(Sprite *sp, const ImVec2 &display_size) {        
        auto style = &ImGui::GetStyle();
        ImVec2 s = ImGui::GetCursorScreenPos();
        ImVec2 min = ImVec2(s.x + style->CellPadding.x, s.y + style->CellPadding.y*2);
        ImVec2 max = ImVec2(min.x + display_size.x, min.y + display_size.y);
        
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRect(min, max,  ImGui::GetColorU32(ImGuiCol_Border), 0, 0, 2.0f);
    }

    void DrawOverlay(Sprite *sp, const ImVec2 &display_size) {
    }

    // Calculate the size of the display area based on the sprite's width and height
    void CalculateDisplaySize(Sprite *sp, ImVec2 &display_size) {
        ImVec2 region = ImGui::GetContentRegionAvail();
        const int widthInPixels = sp->widthInBytes << 3;
        const int heightInPixels = sp->heightInPixels;

        // Scale down the display area slightly
        const float scale = 0.95f; 
        // Ensure the display size is proportional to the sprite's dimensions
        display_size = ImVec2(region.y * widthInPixels/heightInPixels * scale, region.y * scale);
    }
};