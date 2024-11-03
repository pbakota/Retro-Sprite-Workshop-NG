#pragma once
#include "imgui.h"
#include "statusbar.h"
#include "color_selector.h"

extern StatusBar statusbar;
struct SpriteImage
{
    StatusBar *statusbar;

    bool is_mouse_hovering = false;
    const char* prerendingPrecisionValues[3] = {
        "High (8 frames, 1-bit step)",
        "Medium (4 frames, 2-bit step)",
        "Low (2 frames, 4-bit step)"
    };
    const char* spriteWidthValues[8] = {
        "8 pixels, (1 byte)",
        "16 pixels, (2 bytes)",
        "24 pixels, (3 bytes)",
        "32 pixels, (4 bytes)",
        "40 pixels, (5 bytes)",
        "48 pixels, (6 bytes)",
        "56 pixels, (7 bytes)",
        "64 pixels, (8 bytes)",
    };
    const char* byteOrderValues[3] = {
        "Horizontal C64 Sprite",
        "Vertical Software Sprite",
        "Mixed (Character-Based)"
    };
    int one = 1;
    const float cellWidth = 48.0f;
    const float cellHeight = 24.0f;

    size_t test_sprite_width = 16;
    size_t test_sprite_height = 16;
    char test_sprite[16][16] = {
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0}
    };
    bool colorSelectorOpen = false;
    ColorSelector colorSelector;

    Sprite* currentSprite = nullptr;

    SpriteImage(StatusBar *statusbar): statusbar(statusbar) {}

    void render()
    {
        ImGui::SetNextWindowSize(ImVec2(800, 400));
        ImGui::Begin("Sprite Image");
        if(!currentSprite) {

            ImGui::TextUnformatted("no sprite is opened for editing");
            ImGui::Dummy(ImVec2(0, 10.0f));
            ImGui::TextUnformatted("Double-click on an image on the left-side image list.");
            ImGui::End();
            return;
        }
        if (ImGui::BeginTabBar("#spriteImage", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Image"))
            {
                if (ImGui::BeginTable("#spriteProps", 2, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingStretchProp))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 400.0f);
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Width ");
                        ImGui::SameLine(); ImGui::PushID(0); ImGui::SetNextItemWidth(200.0f); if(ImGui::BeginCombo("", spriteWidthValues[currentSprite->widthInBytes-1])) {
                            for (size_t n = 0; n < IM_ARRAYSIZE(spriteWidthValues); n++) {
                                const bool is_selected = (currentSprite->widthInBytes == n+1);
                                if (ImGui::Selectable(spriteWidthValues[n], is_selected)) {
                                    currentSprite->widthInBytes = n+1;
                                }

                                if (is_selected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        } ImGui::PopID();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Byte Order");
                        ImGui::SameLine(); ImGui::PushID(1); ImGui::SetNextItemWidth(200.0f); if(ImGui::BeginCombo("", byteOrderValues[(size_t)currentSprite->byteAligment])) {
                            for (size_t n = 0; n < IM_ARRAYSIZE(byteOrderValues); n++) {
                                const bool is_selected = ((size_t)currentSprite->byteAligment == n);
                                if (ImGui::Selectable(byteOrderValues[n], is_selected)) {
                                    currentSprite->byteAligment = (Sprite::ByteAligment)n;
                                }

                                if (is_selected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        } ImGui::PopID();
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Height");
                        ImGui::SameLine(); ImGui::PushID(2); ImGui::SetNextItemWidth(100); if(ImGui::InputScalar("", ImGuiDataType_U8, &currentSprite->heightInPixels, &one, nullptr, "%d", 0)) {
                            if(currentSprite->heightInPixels < 1) currentSprite->heightInPixels = 1;
                            if(currentSprite->heightInPixels > 64) currentSprite->heightInPixels = 64;
                        } ImGui::PopID();
                        ImGui::TableNextColumn(); ImGui::PushID(3); if(ImGui::Checkbox("Multicolor (2 bits per pixel)", &currentSprite->multicolorMode)) {} ImGui::PopID();
                    }
                    ImGui::EndTable();
                }
                ImageEditor();

                // Drawing color selector
                DrawingColorSelector();

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Metadata and Settings"))
            {
                if (ImGui::BeginTable("#spriteMetadata", 2, ImGuiTableFlags_NoBordersInBody))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Image ID");
                        ImGui::TableNextColumn(); ImGui::PushID(0); ImGui::InputText("", currentSprite->spriteID, IM_ARRAYSIZE(currentSprite->spriteID)); ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Comments");
                        ImGui::TableNextColumn(); ImGui::PushID(1); ImGui::SetNextItemWidth(-1); ImGui::InputTextMultiline("", currentSprite->description, IM_ARRAYSIZE(currentSprite->description)); ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Index");
                        ImGui::TableNextColumn(); ImGui::Text("%d; Char: %d", 0, 0);

                        ImGui::Dummy(ImVec2(0,10.0f));
                        ImGui::SeparatorText("Source Code Generator Settings");
                        ImGui::Dummy(ImVec2(0,10.0f));

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TableNextColumn(); ImGui::PushID(3); if(ImGui::Checkbox("Pre-render Software Sprites", &currentSprite->prerenderSoftwareSprite)) {} ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        ImGui::BeginDisabled(!currentSprite->prerenderSoftwareSprite);
                            const char* combo_preview_value = prerendingPrecisionValues[(size_t)currentSprite->renderingPrecision];
                            ImGui::TableNextColumn(); ImGui::PushID(4); ImGui::SetNextItemWidth(200.0f); if(ImGui::BeginCombo("", combo_preview_value)) {
                                for (size_t n = 0; n < IM_ARRAYSIZE(prerendingPrecisionValues); n++) {
                                    const bool is_selected = ((size_t)currentSprite->renderingPrecision == n);
                                    if (ImGui::Selectable(prerendingPrecisionValues[n], is_selected)) {
                                        currentSprite->renderingPrecision = (Sprite::PrerendingPrecision)n;
                                    }

                                    if (is_selected) {
                                        ImGui::SetItemDefaultFocus();
                                    }
                                }
                                ImGui::EndCombo();
                            } ImGui::PopID();
                        ImGui::EndDisabled();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Final Compiled Size");
                        ImGui::TableNextColumn(); ImGui::Text("%d bytes",
                            (!currentSprite->prerenderSoftwareSprite ? 64: (currentSprite->renderingPrecision == Sprite::PrerendingPrecision::Low2Frames) ? 384: 192));
                    }
                    ImGui::EndTable();
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();
    }

    void ImageEditor() {
        const ImVec2 p = ImGui::GetCursorScreenPos();
        float bx = p.x, by = p.y;

        float cw = cellWidth*(((1+statusbar->zoomIndex)*25))/100.0f, ch = cellHeight*(((1+statusbar->zoomIndex)*25))/100.0f;
        ImVec2 editor_pos = ImVec2(bx, by), editor_size = ImVec2(test_sprite_width*cw+bx, test_sprite_height*ch+by);
        statusbar->is_rowcol_visible = is_mouse_hovering = (ImGui::IsMouseHoveringRect(editor_pos, editor_size));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f,3.0f));
        if(ImGui::BeginChild("#drawing", ImVec2(0, -30.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar)) {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 s = ImGui::GetCursorScreenPos();
            if(is_mouse_hovering) {
                const ImVec2 m = ImGui::GetMousePos();
                statusbar->col = std::floor((m.x - s.x) / cw);
                statusbar->row = std::floor((m.y - s.y) / ch);
            }
            for(size_t y = 0; y < test_sprite_height; ++y) {
                for(size_t x = 0; x < test_sprite_width; ++x) {
                    float xx = s.x + x * cw, yy = s.y + y * ch;
                    drawList->AddRectFilled(ImVec2(xx, yy), ImVec2(xx+cw, yy+ch),
                        test_sprite[y][x] ? ImGui::GetColorU32(ImGuiCol_ButtonActive) : ImGui::GetColorU32(ImGuiCol_Border));
                }
            }
            if(1 /* TODO: make grid visibility configurable */) {
                for(size_t x = 0; x < test_sprite_width; ++x) {
                    ImVec2 p0 = ImVec2(s.x + x * cw, s.y), p1 = ImVec2(s.x + x * cw, s.y + test_sprite_height*ch);
                    drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
                }
                for(size_t y = 0; y < test_sprite_height; ++y) {
                    ImVec2 p0 = ImVec2(s.x, s.y + y*ch), p1 = ImVec2(s.x + test_sprite_width*cw, s.y + y*ch);
                    drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
                }
                for(size_t x = 0; x <= test_sprite_width; x+=4) {
                    ImVec2 p0 = ImVec2(s.x + x * cw, s.y), p1 = ImVec2(s.x + x * cw, s.y + test_sprite_height*ch);
                    drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_PlotLines), 1.0f);
                }
                for(size_t y = 0; y <= test_sprite_height; y+=8) {
                    ImVec2 p0 = ImVec2(s.x, s.y + y*ch), p1 = ImVec2(s.x + test_sprite_width*cw, s.y + y*ch);
                    drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_PlotLines), 1.0f);
                }
            }
            // The trick to fool ImGui to set up the scroll sizes properly,
            // otherwise the scroll bars will not be visible, and we would not be able to scroll the view.
            // the reason is: the drawing with drawlist does not update the cursor position. We have to do that.
            ImGui::SetCursorPos(ImVec2(editor_size.x - editor_pos.x, editor_size.y - editor_pos.y));
            ImGui::EndChild();
        }
        ImGui::PopStyleVar();
    }

    // color byte order: 0xAABBGGRR
    void DrawingColorSelector() {
        ImU32 color;
        bool hovered, is_hovered = false;
        if(DrawColorButton(0, 0xff0000ff, "Background", &hovered)) {
            colorSelectorOpen = true;
            color = 0xff0000ff;
        }
        if(hovered) {
            is_hovered = true;
        }
        if(currentSprite->multicolorMode) {
            ImGui::SameLine(); if(DrawColorButton(1, 0xff00ff00, "Multi 1", &hovered)) {
                colorSelectorOpen = true;
                color = 0xff00ff00;
            }
            if(hovered) {
                is_hovered = true;
            }
            ImGui::SameLine(); if(DrawColorButton(2, 0xffff0000, "Multi 2", &hovered)) {
                colorSelectorOpen = true;
                color = 0xffff0000;
            }
            if(hovered) {
                is_hovered = true;
            }
        }
        ImGui::SameLine(); if(DrawColorButton(3, 0xffffff00, "Character", &hovered)) {
            colorSelectorOpen = true;
            color = 0xffffff00;
        }
        if(hovered) {
            is_hovered = true;
        }

        if(is_hovered) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        if(colorSelector.show(&colorSelectorOpen, color)) {
            ;;
        }
    }

    bool DrawColorButton(ImGuiID id, ImU32 color, const char *caption, bool *p_hovered) {
        ImGui::PushID(id);
        if(is_light_color(color)) {
            // Bright color, use black text
            ImGui::PushStyleColor(ImGuiCol_Text, 0xff000000);
        } else {
            // Dark color, use white text
            ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);
        }
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        // NOTE: ImGuiButtonFlags_NoHighlight is my additional flag, it is not in the official version
        bool pressed = (ImGui::ButtonEx(caption, ImVec2(100,25), ImGuiButtonFlags_NoHighlight));
        bool hovered = ImGui::IsItemHovered();

        *p_hovered = hovered;
        ImGui::PopStyleColor(2);
        ImGui::PopID();

        return pressed;
    }
};
