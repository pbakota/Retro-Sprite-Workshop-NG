#pragma once

#include "imgui.h"
#include "sprite.h"
#include "sprite_manager.h"

extern SDL_Renderer *renderer;

struct Animation {

    int selectedFrameIndex = 0;
    const int one = 1; // Used for input scalar step
    const float scale = 0.8f; // Scale down the display area slightly
    int previewFrameIndex = 0;
    float previewTimer = 0;

    #ifndef USE_CLIPBOARD_FOR_COPY_AND_PASTE
    std::string copyBuffer;
    #endif

    Animation() {}

    void Init(Sprite *sp) {
        if(sp->animationFrames.empty()) {
            selectedFrameIndex = 0;
            AddEmptyFrame(sp, true);
        }
    }

    void ResetAnimation(Sprite *sp) {
        if(!sp->animationAttached) return;
        selectedFrameIndex = 0;
        previewFrameIndex = 0;
        previewTimer = 0;
        std::memcpy((void*)sp->data, (void*)sp->animationFrames[0].data, sizeof(sp->data));
        sp->Invalidate();
    }

    inline bool IsAnimationAttached(Sprite *sp) {
        return sp->animationAttached && !sp->animationFrames.empty();
    }

    inline void UpdateFrameIfAnimated(Sprite *sp) {
        if(sp->animationAttached) UpdateFrame(sp);
    }

    void UpdateAllFramesIfAnimated(Sprite *sp) {
        if(!sp->animationAttached) return;
        for(size_t i=0; i<sp->animationFrames.size(); ++i) {
            auto &frame = sp->animationFrames[i];
            sp->UpdateTextureFromSpriteData(renderer, frame.image, frame.data);
        }
    }

    void UpdateFrame(Sprite *sp) {
        if(selectedFrameIndex < 0 || selectedFrameIndex >= (int)sp->animationFrames.size()) {
            return; // Invalid index
        }
        std::memcpy((void*)sp->animationFrames[selectedFrameIndex].data, (void*)sp->data, sizeof(sp->data));
        sp->UpdateTextureFromSpriteData(renderer, sp->animationFrames[selectedFrameIndex].image, sp->animationFrames[selectedFrameIndex].data);
    }

    char *GetData(Sprite *sp) {
        if(selectedFrameIndex < 0 || selectedFrameIndex >= (int)sp->animationFrames.size()) {
            return nullptr; // Invalid index
        }
        return sp->animationFrames[selectedFrameIndex].data;
    }

    void CopyFrame(Sprite *sp) {
    #ifndef USE_CLIPBOARD_FOR_COPY_AND_PASTE
        copyBuffer = Serializator::Serialize(sp).c_str();
    #else
        SDL_SetClipboardText(Serializator::Serialize(sp).c_str());
    #endif
    }

    void PasteFrame(Sprite *sp) {
        Sprite newSprite;
    #ifndef USE_CLIPBOARD_FOR_COPY_AND_PASTE
        if(copyBuffer.empty()) return;
        Serializator::Deserialize(copyBuffer.c_str(), &newSprite);
    #else
        if (!SDL_HasClipboardText()) return;
        const char *clipboard = SDL_GetClipboardText();
        Serializator::Deserialize(clipboard, &newSprite);
        SDL_free((void*)clipboard);
    #endif
        if(newSprite.widthInBytes != sp->widthInBytes || newSprite.heightInPixels != sp->heightInPixels) return; // Size mismatch, cannot paste
        std::memcpy((void*)sp->data, (void*)newSprite.data, sizeof(sp->data));
        sp->Invalidate();
   }

    void render(SDL_Renderer *render, Sprite *sp) {
        ImGui::SetNextWindowSize(ImVec2(320.0f, 240.0f), ImGuiCond_FirstUseEver);
        if(ImGui::Begin("Animation View", nullptr, ImGuiWindowFlags_None)) {
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
    }

    void AddEmptyFrame(Sprite *sp, bool copyFromSprite = false) {
        auto& frame = sp->animationFrames.empty() ? sp->animationFrames.emplace_back() : *sp->animationFrames.emplace(sp->animationFrames.begin() + selectedFrameIndex + 1);
        frame.image = sp->CreateSpriteImageTexture(renderer);
        if(copyFromSprite) {
            std::memcpy((void*)frame.data, (void*)sp->data, sizeof(sp->data));
        } else {
            std::memset((void*)frame.data, 0, sizeof(frame.data));
        }
        sp->UpdateTextureFromSpriteData(renderer, frame.image, frame.data);
    }

    void DuplicateFrame(Sprite *sp) {
        auto& curr = *(sp->animationFrames.begin() + selectedFrameIndex);
        auto& frame = *sp->animationFrames.emplace(sp->animationFrames.begin() + selectedFrameIndex + 1);
        frame.image = sp->CreateSpriteImageTexture(renderer);
        std::memcpy((void*)frame.data, (void*)curr.data, sizeof(frame.data));
        sp->UpdateTextureFromSpriteData(renderer, frame.image, frame.data);
    }

    void DeleteFrame(Sprite *sp) {
        if(sp->animationFrames.size() <= 1) {
            return; // Cannot delete the last frame
        }
        sp->animationFrames.erase(sp->animationFrames.begin() + selectedFrameIndex);
    }

    void MoveFrame(Sprite *sp, int direction) {
        if(sp->animationFrames.size() <= 1 || selectedFrameIndex < 0 || selectedFrameIndex >= (int)sp->animationFrames.size()) {
            return; // Cannot move if there's only one frame or index is out of bounds
        }
        int newIndex = selectedFrameIndex + direction;
        if(newIndex < 0 || newIndex >= (int)sp->animationFrames.size()) {
            return; // Prevent moving out of bounds
        }
        std::swap(sp->animationFrames[selectedFrameIndex], sp->animationFrames[newIndex]);
        selectedFrameIndex = newIndex; // Update the selected index to the new position
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
                DrawFrame(sp, o, i, display_size);
                if(i == selectedFrameIndex) {
                    DrawCursor(sp, o, display_size);
                }
                bool hovered, held;
                ImVec2 min = o;
                ImVec2 max = ImVec2(min.x + display_size.x*scale, min.y + display_size.y*scale);
                ImGui::PushID(i+1); if(ImGui::ButtonBehavior(ImRect(min,max), ImGui::GetID("##frame"), &hovered, &held, button_flags)) {
                    selectedFrameIndex = i;
                    auto &frame = sp->animationFrames[i];
                    std::memcpy((void*)sp->data, (void*)frame.data, sizeof(sp->data));
                    sp->Invalidate();
                } ImGui::PopID();
                o.x += display_size.x*scale+4.0f; // Add some spacing between frames
                totalWidth += display_size.x*scale+4.0f;
            }
            ImGui::SetCursorPosX(p.x + totalWidth);

            if (ImGui::BeginPopupContextWindow("##animationPopup")) {
                if(ImGui::MenuItem("Add Frame")) {
                    AddEmptyFrame(sp);
                    selectedFrameIndex += 1; // Move to the newly added frame
                }
                if(ImGui::MenuItem("Duplicate Current Frame")) {
                    DuplicateFrame(sp);
                    selectedFrameIndex += 1; // Move to the newly added frame
                }
                if(ImGui::MenuItem("Move Current Frame Left")) {
                    MoveFrame(sp, -1);
                }
                if(ImGui::MenuItem("Move Current Frame Right")) {
                    MoveFrame(sp, 1);
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Copy")) {
                    CopyFrame(sp);
                }
                if(ImGui::MenuItem("Paste")) {
                    PasteFrame(sp);
                    UpdateFrame(sp);
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Delete Current Frame")) {
                    DeleteFrame(sp);
                    if(selectedFrameIndex >= (int)sp->animationFrames.size()) {
                        selectedFrameIndex = (int)sp->animationFrames.size() - 1; // Move to the last frame
                    }
                }
                ImGui::EndPopup();
            }
            ImGui::EndChild();
        }
    }

    void DrawPreview(Sprite *sp, const ImVec2 &display_size) {
        ImGui::PushID(0); ImGui::SetNextItemWidth(80.0f); if(ImGui::InputScalar("", ImGuiDataType_U8, &sp->animationFPS, &one, nullptr, "%d", 0)) {
            if(sp->animationFPS < 1) {
                sp->animationFPS = 1; // Ensure FPS is at least 1
            }
            if(sp->animationFPS > 60) {
                sp->animationFPS = 60; // Cap FPS at 60
            }
        } ImGui::PopID();
        ImGui::SameLine(); ImGui::Text("FPS");

        // Display the current frame
        Sprite::Frame &frame = sp->animationFrames[previewFrameIndex];
        ImVec2 size = ImVec2(display_size.x*scale*0.8, display_size.y*scale*0.8);
        ImVec2 region = ImGui::GetContentRegionAvail();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (region.x - size.x)*0.5);
        // ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (region.y - size.y)*0.5);
        ImGui::Image((ImTextureID)frame.image, size, ImVec2(0, 0), ImVec2((sp->widthInBytes<<3)/64.0f, (float)sp->heightInPixels/64.0f));

        auto dt = ImGui::GetIO().DeltaTime;
        if(dt>1.0f) {
            return;
        }
        previewTimer += dt; // Increment the timer by the delta time
        if(previewTimer >= 1.0f/sp->animationFPS) { // Check if enough time has passed for the next frame
            // Reset the timer and move to the next frame
            previewTimer = previewTimer - 1.0f/sp->animationFPS; // Keep any overflow for the next frame
            previewFrameIndex += 1;
            if(previewFrameIndex >= (int)sp->animationFrames.size()) {
                previewFrameIndex = 0; // Loop back to the first frame
            }
        }
    }

    void DrawFrame(Sprite *sp, const ImVec2 &pos, int index, const ImVec2 &display_size) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        Sprite::Frame &frame = sp->animationFrames[index];
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