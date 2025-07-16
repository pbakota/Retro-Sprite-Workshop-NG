#pragma once

#include "imgui.h"
#include "sprite.h"

extern SDL_Renderer *renderer;

struct Animation {

    int currentFrameIndex = 0;
    const int one = 1; // Used for input scalar step
    const float scale = 0.8f; // Scale down the display area slightly
    int previewFrameIndex = 0;
    int previewTimer = 0;

    Animation() {}

    void Init(Sprite *sp) {
        if(sp->animationFrames.empty()) {
            currentFrameIndex = 0;
            AddEmptyFrame(sp, true);
        }
    }

    void render(SDL_Renderer *render, Sprite *sp) {
        ImGui::SetNextWindowSize(ImVec2(320.0f, 240.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("Animation View");
        if (ImGui::BeginTabBar("#spriteAnimation", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Animation")) {

                ImVec2 display_size;
                CalculateDisplaySize(sp, display_size);

                if (ImGui::BeginTable("#spriteMetadata", 2, ImGuiTableFlags_NoBordersInBody)) {
                    ImGui::TableSetupColumn("Frames", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, display_size.x*scale);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    DrawFrames(sp, display_size);
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

    void AddEmptyFrame(Sprite *sp, bool copyFromSprite = true) {
        auto& frame = *sp->animationFrames.emplace(sp->animationFrames.begin() + currentFrameIndex);
        frame.image = sp->CreateSpriteImageTexture(renderer);
        frame.dirty = false; // Mark as not dirty since we just copied the data
        if(copyFromSprite) {
            std::memcpy((void*)frame.data, (void*)sp->data, sizeof(sp->data));
            sp->UpdateTextureFromSpriteData(renderer, frame.image, frame.data);
        } else {
            std::memset((void*)frame.data, 0, sizeof(frame.data));
        }
    }

    void CopyPrevFrame(Sprite *sp) {
        auto& prev = *(sp->animationFrames.begin() + currentFrameIndex);
        auto& frame = *sp->animationFrames.emplace(sp->animationFrames.begin() + currentFrameIndex);
        frame.dirty = false; // Mark as not dirty since we just copied the data
        std::memcpy((void*)frame.data, (void*)&prev.data, sizeof(prev.data));
        sp->UpdateTextureFromSpriteData(renderer, frame.image, frame.data);
    }

    void DrawFrames(Sprite *sp, const ImVec2 &display_size) {
        if(ImGui::BeginChild("Frames", ImVec2(0,0), 0, ImGuiWindowFlags_HorizontalScrollbar)) {

            const int button_flags = ImGuiButtonFlags_PressedOnClick;
            const int noFrames = sp->animationFrames.size();

            ImVec2 region = ImGui::GetContentRegionAvail();
            ImVec2 s = ImGui::GetCursorScreenPos();
            ImVec2 p = ImGui::GetCursorPos();
            ImVec2 o = ImVec2(s.x, s.y + (region.y-display_size.y*scale)*0.5f);

            float totalWidth = 0;
            for(int i=0; i<noFrames; ++i) {
                DrawFrame(sp, o, i+1, display_size);
                if(i == currentFrameIndex) {
                    DrawCursor(sp, o, display_size);
                }
                bool hovered, held;
                ImVec2 min = o;
                ImVec2 max = ImVec2(min.x + display_size.x*scale, min.y + display_size.y*scale);
                ImGui::PushID(i+1); if(ImGui::ButtonBehavior(ImRect(min,max), ImGui::GetID("##frame"), &hovered, &held, button_flags)) {
                    currentFrameIndex = i;
                } ImGui::PopID();
                if(hovered) {

                }
                o.x += display_size.x*scale+4.0f; // Add some spacing between frames
                totalWidth += display_size.x*scale+4.0f;
            }
            ImGui::SetCursorPosX(p.x + totalWidth);

            if (ImGui::BeginPopupContextWindow("##animationPopup")) {
                if(ImGui::MenuItem("Add Frame")) {
                    AddEmptyFrame(sp);
                    currentFrameIndex += 1; // Move to the newly added frame
                }
                if(ImGui::MenuItem("Duplicate Current Frame")) {
                    // FIXME: This should copy the current frame
                    // CopyPrevFrame(sp);
                    // currentFrameIndex += 1; // Move to the newly added frame
                }
                if(ImGui::MenuItem("Move Current Frame Left")) {

                }
                if(ImGui::MenuItem("Move Current Frame Right")) {

                }
                ImGui::Separator();
                if(ImGui::MenuItem("Delete Current Frame")) {

                }
                ImGui::EndPopup();
            }

            // } ImGui::PopID();
            ImGui::EndChild();
        }
    }

    void DrawPreview(Sprite *sp, const ImVec2 &display_size) {
        ImGui::PushID(0); ImGui::SetNextItemWidth(80.0f); if(ImGui::InputScalar("", ImGuiDataType_U8, &sp->animationFPS, &one, nullptr, "%d", 0)) {
            if(sp->animationFPS < 1) {
                sp->animationFPS = 1; // Ensure FPS is at least 1
            }
            if(sp->animationFPS > 50) {
                sp->animationFPS = 50; // Cap FPS at 50
            }
        } ImGui::PopID();
        ImGui::SameLine(); ImGui::Text("FPS");

        // Display the current frame
        Sprite::Frame &frame = sp->animationFrames[previewFrameIndex];
        ImVec2 size = ImVec2(display_size.x*scale, display_size.y*scale);
        ImGui::Image((ImTextureID)frame.image, size, ImVec2(0, 0), ImVec2((sp->widthInBytes<<3)/64.0f, (float)sp->heightInPixels/64.0f));

        previewTimer ++;
        if((int)(1.0/previewTimer) == sp->animationFPS) {
            previewTimer = 0;
            previewFrameIndex += 1;
            if(previewFrameIndex >= (int)sp->animationFrames.size()) {
                previewFrameIndex = 0; // Loop back to the first frame
            }
        }
    }

    void DrawFrame(Sprite *sp, const ImVec2 &pos, int index, const ImVec2 &display_size) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        Sprite::Frame &frame = sp->animationFrames[currentFrameIndex];
        ImVec2 size = ImVec2(display_size.x*scale, display_size.y*scale);
        dl->AddImage((ImTextureID)frame.image, pos, ImVec2(pos.x+size.x, pos.y+size.y), ImVec2(0, 0), ImVec2((sp->widthInBytes<<3)/64.0f, (float)sp->heightInPixels/64.0f));
        ImVec2 w = ImGui::CalcTextSize(std::to_string(index).c_str());
        dl->AddRectFilled(pos, ImVec2(pos.x + w.x + 2.0f, pos.y + 14.0f + 2.0f), ImGui::GetColorU32(ImGuiCol_WindowBg));
        dl->AddText(ImVec2(pos.x+2.0f, pos.y+2.0f), ImGui::GetColorU32(ImGuiCol_Text), std::to_string(index).c_str());
    }

    void DrawCursor(Sprite *sp, ImVec2 pos, const ImVec2 &display_size) {
        ImVec2 min = ImVec2(pos.x, pos.y);
        ImVec2 max = ImVec2(min.x + display_size.x*scale -1.0f, min.y + display_size.y*scale-1.0f);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRect(min, max,  ImGui::GetColorU32(ImGuiCol_Text), 0, 0, 3.0f);
    }

    void DrawOverlay(Sprite *sp, const ImVec2 &display_size) {
    }

    // Calculate the size of the display area based on the sprite's width and height
    void CalculateDisplaySize(Sprite *sp, ImVec2 &display_size) {
        ImVec2 region = ImGui::GetContentRegionAvail();
        const int widthInPixels = sp->widthInBytes << 3;
        const int heightInPixels = sp->heightInPixels;

        // Scale down the display area slightly
        const float adjust = 1.0f;
        // Ensure the display size is proportional to the sprite's dimensions
        display_size = ImVec2(region.y * widthInPixels/heightInPixels * adjust, region.y * adjust);
    }
};