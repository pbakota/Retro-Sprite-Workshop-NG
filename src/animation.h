#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include "sprite.h"


struct Animation {

    int currentFrameIndex = 0;

    Animation() {}

    void render(SDL_Renderer *render, Sprite *sp) {
        ImGui::SetNextWindowSize(ImVec2(320.0f, 240.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("Animation View");
        if (ImGui::BeginTabBar("#spriteAnimation", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Animation")) {

                DrawCursor(sp);


                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();
    }

    void DrawCursor(Sprite *sp) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 s = ImGui::GetCursorScreenPos();

        const int widthInPixels = sp->widthInBytes<<3;
        ImVec2 display_size = ImVec2(widthInPixels *4.0f, sp->heightInPixels *4.0f);

        s.x += display_size.x * currentFrameIndex;
        auto min = s;
        auto max = s + display_size;
        dl->AddRect(min, max, ImGuiCol_Border);
    }
};