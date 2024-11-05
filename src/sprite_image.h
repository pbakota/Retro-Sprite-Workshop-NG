#pragma once
#include "imgui.h"
#include "statusbar.h"
#include "color_selector.h"

extern StatusBar statusbar;
struct SpriteImage
{
    StatusBar *statusbar;
    Sprite* currentSprite = nullptr;

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
    const float cellSize = 24.0f;
    bool colorSelectorOpen = false;
    ColorSelector colorSelector;
    ImGuiID selectedColor = 0;

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
                                    currentSprite->Invalidate();
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

                            currentSprite->Invalidate();
                        } ImGui::PopID();
                        ImGui::TableNextColumn(); ImGui::PushID(3); if(ImGui::Checkbox("Multicolor (2 bits per pixel)", &currentSprite->multicolorMode)) {
                            selectedColor = 0; // Reset drawing color to background
                            currentSprite->Invalidate();
                        } ImGui::PopID();
                    }
                    ImGui::EndTable();
                }

                ImageEditor();

                // Store current zoom index
                // FIXME: Somehow update this value when it was really changed
                currentSprite->zoomIndex = statusbar->zoomIndex;

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
        size_t widthInPixels = (currentSprite->widthInBytes<<3);

        const float cs = cellSize*(((1+statusbar->zoomIndex)*25))/100.0f;
        ImVec2 editor_pos = ImVec2(bx, by), editor_size = ImVec2(widthInPixels*cs+bx, currentSprite->heightInPixels*cs+by);
        statusbar->is_rowcol_visible = is_mouse_hovering = (ImGui::IsMouseHoveringRect(editor_pos, editor_size)) && !ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f,3.0f));
        if(ImGui::BeginChild("#drawing", ImVec2(0, -30.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar)) {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 s = ImGui::GetCursorScreenPos();
            if(is_mouse_hovering) {
                const ImVec2 m = ImGui::GetMousePos();
                statusbar->col = std::floor((m.x - s.x) / cs);
                statusbar->row = std::floor((m.y - s.y) / cs);
            }
            for(size_t y = 0; y < currentSprite->heightInPixels; ++y) {
                if(currentSprite->multicolorMode) {
                    for(size_t x = 0; x < widthInPixels; x+=2) {
                        ImU32 pixelColor;
                        size_t c = (((char*)currentSprite->data)[y*currentSprite->pitch + x + 0] << 1) | ((char*)currentSprite->data)[y*currentSprite->pitch + x + 1];
                        // std::cerr << std::hex << c << std::endl;
                        switch(c) {
                            case 0: pixelColor = currentSprite->backgroundColor; break;
                            case 1: pixelColor = currentSprite->multi1Color; break;
                            case 2: pixelColor = currentSprite->multi2Color; break;
                            case 3: pixelColor = currentSprite->characterColor; break;
                        }
                        float xx = s.x + x * cs, yy = s.y + y * cs;
                        drawList->AddRectFilled(ImVec2(xx, yy), ImVec2(xx+cs*2, yy+cs), pixelColor);
                    }
                } else {
                    for(size_t x = 0; x < widthInPixels; ++x) {
                        float xx = s.x + x * cs, yy = s.y + y * cs;
                        drawList->AddRectFilled(ImVec2(xx, yy), ImVec2(xx+cs, yy+cs), ((char*)currentSprite->data)[y*currentSprite->pitch + x]
                            ? currentSprite->characterColor : currentSprite->backgroundColor);
                    }
                }
            }
            if(1 /* TODO: make grid visibility configurable */) {
                if(currentSprite->multicolorMode) {
                    for(size_t x = 0; x <= widthInPixels; x+=2) {
                        ImVec2 p0 = ImVec2(s.x + x * cs, s.y), p1 = ImVec2(s.x + x * cs, s.y + currentSprite->heightInPixels*cs);
                        drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
                    }
                } else {
                    for(size_t x = 0; x <= widthInPixels; ++x) {
                        ImVec2 p0 = ImVec2(s.x + x * cs, s.y), p1 = ImVec2(s.x + x * cs, s.y + currentSprite->heightInPixels*cs);
                        drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
                    }
                }
                for(size_t y = 0; y < currentSprite->heightInPixels; ++y) {
                    ImVec2 p0 = ImVec2(s.x, s.y + y*cs), p1 = ImVec2(s.x + widthInPixels*cs, s.y + y*cs);
                    drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
                }
                for(size_t x = 0; x <= widthInPixels; x+=8) {
                    ImVec2 p0 = ImVec2(s.x + x * cs, s.y), p1 = ImVec2(s.x + x * cs, s.y + currentSprite->heightInPixels*cs);
                    drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_PlotLines), 1.0f);
                }
                for(size_t y = 0; y <= currentSprite->heightInPixels; y+=8) {
                    ImVec2 p0 = ImVec2(s.x, s.y + y*cs), p1 = ImVec2(s.x + widthInPixels*cs, s.y + y*cs);
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
    ImU32 *color = nullptr;
    void DrawingColorSelector() {
        bool hovered, is_hovered = false;
        if(DrawColorButton(0, currentSprite->backgroundColor, "Background", &hovered)) {
            if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                colorSelectorOpen = true;
                color = &currentSprite->backgroundColor;
            } else {
                selectedColor = 0;
            }
        }
        if(hovered) {
            is_hovered = true;
        }
        if(currentSprite->multicolorMode) {
            ImGui::SameLine(); if(DrawColorButton(1, currentSprite->multi1Color, "Multi 1", &hovered)) {
                if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    colorSelectorOpen = true;
                    color = &currentSprite->multi1Color;
                } else {
                    selectedColor = 1;
                }
            }
            if(hovered) {
                is_hovered = true;
            }
            ImGui::SameLine(); if(DrawColorButton(2, currentSprite->multi2Color, "Multi 2", &hovered)) {
                if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    colorSelectorOpen = true;
                    color = &currentSprite->multi2Color;
                } else {
                    selectedColor = 2;
                }
            }
            if(hovered) {
                is_hovered = true;
            }
        }
        ImGui::SameLine(); if(DrawColorButton(3, currentSprite->characterColor, "Character", &hovered)) {
            if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                colorSelectorOpen = true;
                color = &currentSprite->characterColor;
            } else {
                selectedColor = 3;
            }
        }
        if(hovered) {
            is_hovered = true;
        }

        if(is_hovered) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        if(colorSelectorOpen) {
            if(colorSelector.show(&colorSelectorOpen, *color, &currentSprite->palette)) {
                *color = colorSelector.selectedColor;
                currentSprite->Invalidate();
            }
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
        ImVec2 buttonPos = ImGui::GetCursorPos();
        // NOTE: ImGuiButtonFlags_NoHighlight is my additional flag, it is not in the official version
        bool pressed = (ImGui::ButtonEx(caption, ImVec2(100,25), ImGuiButtonFlags_NoHighlight));
        bool hovered = ImGui::IsItemHovered();

        *p_hovered = hovered;
        ImGui::PopStyleColor(2);

        if(selectedColor == id) {
            ImVec2 oldPos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(buttonPos);
            const ImVec2 p = ImGui::GetCursorScreenPos();
            ImDrawList *draw_list = ImGui::GetWindowDrawList();

            ImVec2 p1(p.x, p.y),p2(p.x, p.y+15.0f),p3(p.x+15.0f, p.y);
            draw_list->AddTriangleFilled(p1,p2,p3, is_light_color(color) ? 0xff000000 : 0xffffffff);

            ImGui::SetCursorPos(oldPos);
        }

        ImGui::PopID();

        return pressed;
    }
};
