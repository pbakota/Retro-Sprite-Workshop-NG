#pragma once

#include <imgui.h>

struct TiledView
{
    void render(SDL_Renderer *renderer, Sprite *sprite)
    {
        ImGui::SetNextWindowSize(ImVec2(320.0f, 240.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("Tiled View");
        auto region = ImGui::GetContentRegionAvail();
        const int widthInPixels = sprite->widthInBytes<<3;
        ImVec2 display_size = ImVec2(widthInPixels *4.0f, sprite->heightInPixels *4.0f);
        const int possible_columns = (size_t)std::floor(region.x / display_size.x);
        const int possible_rows = (size_t)std::floor(region.y / display_size.y);
        auto pos = ImGui::GetCursorPos();
        for(auto y=0; y<possible_rows; ++y) {
            float savedX = pos.x;
            for(auto x=0; x<possible_columns; ++x) {
                ImGui::SetCursorPos(pos);
                ImGui::Image((ImTextureID)sprite->GetTextureFixedSize(renderer, display_size), display_size);
                pos.x += display_size.x;
            }
            pos.x = savedX;
            pos.y += display_size.y;
        }
        ImGui::End();
    }
};
