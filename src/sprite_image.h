#pragma once
#include "imgui.h"
#include "project_sprites.h"
#include "sprite_manager.h"
#include "statusbar.h"
#include "color_selector.h"
#include "animation.h"
#include "util.h"

extern StatusBar statusbar;
extern Animation animation;

struct SpriteImage
{
    SpriteManager *spriteManager;
    ProjectSprites *projectSprites;
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
    int heightPixelStep = 1;
    const float cellSize = 24.0f;
    bool colorSelectorOpen = false;
    ColorSelector colorSelector;
    ImGuiID selectedColor = 0;

    const ImU32 MASK_TRANSPARENT = 0xff800080;
    const ImU32 MASK_FILL = 0xff000000;

    enum class Editing {
      Image = 0,
      Mask = 1
    };
    Editing editing = Editing::Image;

    SpriteImage(SpriteManager *spriteManager, ProjectSprites *projectSprites, StatusBar *statusbar): spriteManager(spriteManager), projectSprites(projectSprites), statusbar(statusbar) {}

    void render()
    {
        ImGui::SetNextWindowSize(ImVec2(800, 400));
        ImGui::Begin("Sprite Image");
        if(!spriteManager->currentSprite) {

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
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 300.0f);
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Width ");
                        ImGui::SameLine(); ImGui::PushID(0); ImGui::SetNextItemWidth(200.0f); if(ImGui::BeginCombo("", spriteWidthValues[spriteManager->currentSprite->widthInBytes-1])) {
                            for (size_t n = 0; n < IM_ARRAYSIZE(spriteWidthValues); n++) {
                                const bool is_selected = (spriteManager->currentSprite->widthInBytes == n+1);
                                if (ImGui::Selectable(spriteWidthValues[n], is_selected)) {
                                    spriteManager->currentSprite->widthInBytes = n+1;
                                    spriteManager->currentSprite->Invalidate();
                                    animation.UpdateAllFramesIfAnimated(spriteManager->currentSprite);
                                }
                                if (is_selected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        } ImGui::PopID();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Byte Order");
                        ImGui::SameLine(); ImGui::PushID(1); ImGui::SetNextItemWidth(200.0f); if(ImGui::BeginCombo("", byteOrderValues[(size_t)spriteManager->currentSprite->byteAligment])) {
                            for (size_t n = 0; n < IM_ARRAYSIZE(byteOrderValues); n++) {
                                const bool is_selected = ((size_t)spriteManager->currentSprite->byteAligment == n);
                                if (ImGui::Selectable(byteOrderValues[n], is_selected)) {
                                    spriteManager->currentSprite->byteAligment = (Sprite::ByteAligment)n;
                                }
                                if (is_selected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        } ImGui::PopID();
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Height");
                        heightPixelStep = spriteManager->currentSprite->byteAligment == Sprite::ByteAligment::Mixed_Character_Based ? 8 : 1;
                        ImGui::SameLine(); ImGui::PushID(2); ImGui::SetNextItemWidth(100); 
                          if(ImGui::InputScalar("", ImGuiDataType_U8, &spriteManager->currentSprite->heightInPixels, &heightPixelStep, nullptr, "%d", 0)) {
                            if(spriteManager->currentSprite->byteAligment == Sprite::ByteAligment::Mixed_Character_Based) {
                                if(spriteManager->currentSprite->heightInPixels < 8) spriteManager->currentSprite->heightInPixels = 8;
                            } else {
                                if(spriteManager->currentSprite->heightInPixels < 1) spriteManager->currentSprite->heightInPixels = 1;
                            }
                            if(spriteManager->currentSprite->heightInPixels > 64) spriteManager->currentSprite->heightInPixels = 64;

                            spriteManager->currentSprite->Invalidate();
                            animation.UpdateAllFramesIfAnimated(spriteManager->currentSprite);
                        } ImGui::PopID();
                        ImGui::TableNextColumn(); ImGui::PushID(3); if(ImGui::Checkbox("Multicolor (2 bits per pixel)", &spriteManager->currentSprite->multicolorMode)) {
                            selectedColor = 0; // Reset drawing color to background
                            spriteManager->currentSprite->Invalidate();
                            animation.UpdateAllFramesIfAnimated(spriteManager->currentSprite);
                        } ImGui::PopID();
                        ImGui::PushID(4); if(ImGui::Checkbox("Animated", &spriteManager->currentSprite->animationAttached)) {
                            if(spriteManager->currentSprite->animationAttached) {
                                // Init animation
                                animation.Init(spriteManager->currentSprite);
                            }
                        } ImGui::PopID();
                        ImGui::SameLine();
                        ImGui::PushID(5); if(ImGui::Checkbox("Masked", &spriteManager->currentSprite->masked)) {
                            // Enabled mask
                        } ImGui::PopID();
                        if(spriteManager->currentSprite->masked) {
                            ImGui::SameLine(210.0f); ImGui::TextUnformatted("Editing"); ImGui::SetNextItemWidth(-FLT_MIN);
                            ImGui::SameLine(); if(ImGui::RadioButton("Image", (int*)&editing, (int)Editing::Image)) {}
                            ImGui::SameLine(); if(ImGui::RadioButton("Mask", (int*)&editing, (int)Editing::Mask)) {}
                            ImGui::SameLine(); if(ImGui::Button("Generate Mask")) {}
                        }
                    }
                    ImGui::EndTable();
                }

                ImageEditor();

                // Store current zoom index
                // FIXME: Somehow update this value when it was really changed
                spriteManager->currentSprite->zoomIndex = statusbar->zoomIndex;

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
                        ImGui::TableNextColumn(); ImGui::PushID(0); ImGui::InputText("", spriteManager->currentSprite->spriteID, IM_ARRAYSIZE(spriteManager->currentSprite->spriteID)); ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Comments");
                        ImGui::TableNextColumn(); ImGui::PushID(1); ImGui::SetNextItemWidth(-1); ImGui::InputTextMultiline("", spriteManager->currentSprite->description, IM_ARRAYSIZE(spriteManager->currentSprite->description)); ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Index");
                        // FIXME: Get somehow the real values for index and offset
                        ImGui::TableNextColumn(); ImGui::Text("%lu; Char: %lu", (unsigned long)spriteManager->currentSprite->charIndex, (unsigned long)spriteManager->currentSprite->charOffset);

                        ImGui::Dummy(ImVec2(0,10.0f));
                        ImGui::SeparatorText("Source Code Generator Settings");
                        ImGui::Dummy(ImVec2(0,10.0f));

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TableNextColumn(); ImGui::PushID(3); if(ImGui::Checkbox("Pre-render Software Sprites", &spriteManager->currentSprite->prerenderSoftwareSprite)) {} ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        ImGui::BeginDisabled(!spriteManager->currentSprite->prerenderSoftwareSprite);
                            if(spriteManager->currentSprite->multicolorMode && spriteManager->currentSprite->renderingPrecision == Sprite::PrerendingPrecision::High8Frames) {
                                spriteManager->currentSprite->renderingPrecision = Sprite::PrerendingPrecision::Medium4Frames;
                            }
                            const char* combo_preview_value = prerendingPrecisionValues[(size_t)spriteManager->currentSprite->renderingPrecision];
                            ImGui::TableNextColumn(); ImGui::PushID(4); ImGui::SetNextItemWidth(200.0f); if(ImGui::BeginCombo("", combo_preview_value)) {
                                size_t startOption = spriteManager->currentSprite->multicolorMode ? 1 : 0;
                                for (size_t n = startOption; n < IM_ARRAYSIZE(prerendingPrecisionValues); n++) {
                                    const bool is_selected = ((size_t)spriteManager->currentSprite->renderingPrecision == n);
                                    if (ImGui::Selectable(prerendingPrecisionValues[n], is_selected)) {
                                        spriteManager->currentSprite->renderingPrecision = (Sprite::PrerendingPrecision)n;
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
                        ImGui::TableNextColumn(); ImGui::Text("%lu bytes", (unsigned long)spriteManager->currentSprite->GetByteSize());
                    }
                    ImGui::EndTable();
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();

        if(spriteManager->currentSprite->animationAttached) {
            animation.render(renderer, spriteManager->currentSprite);
        }
    }

    void SetPixel(bool masking, int x, int y, int color) {
        if(masking) {
          spriteManager->currentSprite->SetMask(x, y, color);
        } else {
          spriteManager->currentSprite->SetPixel(x, y, color); spriteManager->currentSprite->Invalidate();
        }
        animation.UpdateFrameIfAnimated(spriteManager->currentSprite);
    }

    void ImageEditor() {

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f,3.0f));
        if(ImGui::BeginChild("#drawing", ImVec2(0, -30.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar)) {
            ImVec2 s = ImGui::GetCursorScreenPos();

            size_t widthInPixels = (spriteManager->currentSprite->widthInBytes<<3);
            const float cs = cellSize*(((1+statusbar->zoomIndex)*25))/100.0f;
            const bool masking = (spriteManager->currentSprite->masked && editing == Editing::Mask);

            float bx = s.x, by = s.y;
            ImVec2 editor_pos = ImVec2(bx, by), editor_size = ImVec2(widthInPixels*cs+bx, spriteManager->currentSprite->heightInPixels*cs+by);
            statusbar->is_rowcol_visible = is_mouse_hovering = (ImGui::IsMouseHoveringRect(editor_pos, editor_size)) && !ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId);
            if(is_mouse_hovering && ImGui::IsWindowFocused()) {
                const ImVec2 m = ImGui::GetMousePos();
                size_t position_x = (size_t)std::floor((m.x - s.x) / (cs * (spriteManager->currentSprite->multicolorMode ? 2 : 1)));
                size_t position_y = std::floor((m.y - s.y) / cs);

                statusbar->col = position_x;
                statusbar->row = position_y;
                statusbar->byteIndex = spriteManager->currentSprite->GetByteIndex(statusbar->row, statusbar->col);
                statusbar->charIndex = spriteManager->currentSprite->GetCharIndex(statusbar->row, statusbar->col);

                // Left mouse button set the pixel to selected color
                if(ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    if(ImGui::IsKeyDown(ImGuiKey_ModShift)) {
                        // With shift always set pixel to background
                        SetPixel(masking, position_x, position_y, 0);
                    } else {
                        if(spriteManager->currentSprite->multicolorMode) {
                            switch(selectedColor) {
                                case 0: {
                                    SetPixel(masking, position_x, position_y, 0);
                                } break;
                                case 1: {
                                    if(masking) {
                                      // NOP
                                    } else {
                                      SetPixel(false, position_x, position_y, 1);
                                    }
                                } break;
                                case 2: {
                                    if(masking) {
                                      // NOP
                                    } else {
                                      SetPixel(false, position_x, position_y, 2);
                                    }
                                } break;
                                case 3: {
                                    SetPixel(masking, position_x, position_y, 3);
                                } break;
                            }
                        } else {
                            switch(selectedColor) {
                                case 0: {
                                    SetPixel(masking, position_x, position_y, 0);
                                } break;
                                case 3: {
                                    SetPixel(masking, position_x, position_y, 1);
                                } break;
                            }
                        }
                    }
                }
            }

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            Sprite *sp = spriteManager->currentSprite;
            for(size_t y = 0; y < sp->heightInPixels; ++y) {
                if(sp->multicolorMode) {
                    for(size_t x = 0; x < widthInPixels; x+=2) {
                        ImU32 pixelColor;
                        size_t c = (((char*)sp->data)[y*sp->pitch + x + 0] << 1) | ((char*)sp->data)[y*sp->pitch + x + 1];
                        switch(c) {
                            case 0: pixelColor = sp->backgroundColor; break;
                            case 1: pixelColor = sp->multi1Color; break;
                            case 2: pixelColor = sp->multi2Color; break;
                            case 3: pixelColor = sp->characterColor; break;
                        }
                        float xx = s.x + x * cs, yy = s.y + y * cs;
                        bool drawPixel = true;
                        if(c == 0 && animation.IsAnimationAttached(sp) && animation.selectedFrameIndex && !masking) {
                            auto &frame = sp->animationFrames[animation.selectedFrameIndex - 1];
                            c = frame.data[y*sp->pitch + x + 0] << 1 | frame.data[y*sp->pitch + x + 1];
                            if(c != 0) {
                                switch(c) {
                                    // case 0: pixelColor = sp->backgroundColor; break;
                                    case 1: pixelColor = sp->multi1Color; break;
                                    case 2: pixelColor = sp->multi2Color; break;
                                    case 3: pixelColor = sp->characterColor; break;
                                }
                                // Dim the pixel color if it's not the background color
                                auto dimmedColor = dim_color(pixelColor, 0.25f);
                                drawList->AddRectFilled(ImVec2(xx, yy), ImVec2(xx+cs*2, yy+cs), dimmedColor);
                                drawPixel = false; // Don't draw the pixel, just the dimmed rectangle
                            }
                        }
                        if(masking) {
                            size_t m = sp->mask[y*sp->pitch + x + 0] << 1 | sp->mask[y*sp->pitch + x + 1];
                            if(m != 0) {
                                pixelColor = MASK_TRANSPARENT;
                            }
                        }
                        if(drawPixel) {
                            drawList->AddRectFilled(ImVec2(xx, yy), ImVec2(xx+cs*2, yy+cs), pixelColor);
                        }
                    }
                } else {
                    for(size_t x = 0; x < widthInPixels; ++x) {
                        ImU32 pixelColor;
                        size_t c = ((char*)sp->data)[y*sp->pitch + x];
                        switch(c) {
                            case 0: pixelColor = sp->backgroundColor; break;
                            case 1: pixelColor = sp->characterColor; break;
                        }
                        float xx = s.x + x * cs, yy = s.y + y * cs;
                        bool drawPixel = true;
                        if(c == 0) {
                            if(animation.IsAnimationAttached(sp) && animation.selectedFrameIndex && !masking) {
                                auto &frame = sp->animationFrames[animation.selectedFrameIndex - 1];
                                c = frame.data[y*sp->pitch + x + 0];
                                if(c!=0) {
                                    switch(c) {
                                        case 0: pixelColor = sp->backgroundColor; break;
                                        case 1: pixelColor = sp->characterColor; break;
                                    }
                                    // Dim the pixel color if it's not the background color
                                    auto dimmedColor = dim_color(pixelColor, 0.25f);
                                    drawList->AddRectFilled(ImVec2(xx, yy), ImVec2(xx+cs, yy+cs), dimmedColor);
                                    drawPixel = false; // Don't draw the pixel, just the dimmed rectangle
                                }
                            }
                        }
                        if(masking) {
                            size_t m = sp->mask[y*sp->pitch + x + 0];
                            if(m != 0) {
                                pixelColor = MASK_TRANSPARENT;
                            }
                        }
                        if(drawPixel) {
                            drawList->AddRectFilled(ImVec2(xx, yy), ImVec2(xx+cs, yy+cs), pixelColor);
                        }
                    }
                }
            if(1 /* TODO: make grid visibility configurable */) {
            }
                if(sp->multicolorMode) {
                    for(size_t x = 0; x <= widthInPixels; x+=2) {
                        ImVec2 p0 = ImVec2(s.x + x * cs, s.y), p1 = ImVec2(s.x + x * cs, s.y + sp->heightInPixels*cs);
                        drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
                    }
                } else {
                    for(size_t x = 0; x <= widthInPixels; ++x) {
                        ImVec2 p0 = ImVec2(s.x + x * cs, s.y), p1 = ImVec2(s.x + x * cs, s.y + sp->heightInPixels*cs);
                        drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
                    }
                }
                for(size_t y = 0; y < sp->heightInPixels; ++y) {
                    ImVec2 p0 = ImVec2(s.x, s.y + y*cs), p1 = ImVec2(s.x + widthInPixels*cs, s.y + y*cs);
                    drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
                }
                for(size_t x = 0; x <= widthInPixels; x+=8) {
                    ImVec2 p0 = ImVec2(s.x + x * cs, s.y), p1 = ImVec2(s.x + x * cs, s.y + sp->heightInPixels*cs);
                    drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_PlotLines), 1.0f);
                }
                for(size_t y = 0; y <= sp->heightInPixels; y+=8) {
                    ImVec2 p0 = ImVec2(s.x, s.y + y*cs), p1 = ImVec2(s.x + widthInPixels*cs, s.y + y*cs);
                    drawList->AddLine(p0, p1, ImGui::GetColorU32(ImGuiCol_PlotLines), 1.0f);
                }
            }
            if (ImGui::BeginPopupContextWindow("##spriteImagePopup"))
            {
                if(ImGui::BeginMenu("Clear Image")) {
                    if(ImGui::MenuItem("Fill with Background Color")) {
                        projectSprites->Action_ClearSprite(0);
                    }
                    if(ImGui::MenuItem("Fill with Multi1 Color")) {
                        projectSprites->Action_ClearSprite(1);
                    }
                    if(ImGui::MenuItem("Fill with Multi2 Color")) {
                        projectSprites->Action_ClearSprite(2);
                    }
                    if(ImGui::MenuItem("Fill with Character Color")) {
                        projectSprites->Action_ClearSprite(3);
                    }
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Flip Image")) {
                    if(ImGui::MenuItem("Horizontally", "Ctrl+Right")) {
                        projectSprites->Action_FlipImage_Horizontal();
                    }
                    if(ImGui::MenuItem("Vertically", "Ctrl+Up")) {
                        projectSprites->Action_FlipImage_Vertical();
                    }
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Shift Image")) {
                    if(ImGui::MenuItem("Up", "Shift+Up")) {
                        projectSprites->Action_ShiftImage_Up(spriteManager->shiftRollingAround);
                    }
                    if(ImGui::MenuItem("Down", "Shift+Down")) {
                        projectSprites->Action_ShiftImage_Down(spriteManager->shiftRollingAround);
                    }
                    if(ImGui::MenuItem("Left", "Shift+Left")) {
                        projectSprites->Action_ShiftImage_Left(spriteManager->shiftRollingAround);
                    }
                    if(ImGui::MenuItem("Right", "Shift+Right")) {
                        projectSprites->Action_ShiftImage_Right(spriteManager->shiftRollingAround);
                    }
                    ImGui::Separator();
                    if(ImGui::Checkbox("Rolling around", &spriteManager->shiftRollingAround)) {}
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Rotate Image")) {
                    if(ImGui::MenuItem("Clockwise", "Alt+Right")) {
                        projectSprites->Action_RotateImage_Clockwise();
                    }
                    if(ImGui::MenuItem("Counter-clockwise", "Alt+Left")) {
                        projectSprites->Action_RotateImage_CounterClockwise();
                    }
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Capture Sprite from Screenshot ...", "F6")) {
                    projectSprites->Action_Capture();
                }
                ImGui::Separator();
                if(ImGui::BeginMenu("Row")) {
                    if(ImGui::MenuItem("Inset Row")) {
                        projectSprites->Action_InsertRow(statusbar->row);
                    }
                    if(ImGui::MenuItem("Remove Row")) {
                        projectSprites->Action_RemoveRow(statusbar->row);
                    }
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Column")) {
                    if(ImGui::MenuItem("Inset Column")) {
                        projectSprites->Action_InsertColumn(statusbar->col);
                    }
                    if(ImGui::MenuItem("Remove Column")) {
                        projectSprites->Action_RemoveColumn(statusbar->col);
                    }
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Rearrange Color Pixels...")) {
                    projectSprites->Action_RearrangeColors();
                }
                ImGui::EndPopup();
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
        const bool masking = (spriteManager->currentSprite->masked && editing == Editing::Mask);

        if(DrawColorButton(0, (masking ? MASK_FILL : spriteManager->currentSprite->backgroundColor), (masking ? "Image" : "Background"), &hovered)) {
            if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                colorSelectorOpen = true;
                color = &spriteManager->currentSprite->backgroundColor;
            } else {
                selectedColor = 0;
            }
        }
        if(hovered) {
            is_hovered = true;
        }
        if(!masking && spriteManager->currentSprite->multicolorMode) {
            ImGui::SameLine(); if(DrawColorButton(1, spriteManager->currentSprite->multi1Color, "Multi 1", &hovered)) {
                if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    colorSelectorOpen = true;
                    color = &spriteManager->currentSprite->multi1Color;
                } else {
                    selectedColor = 1;
                }
            }
            if(hovered) {
                is_hovered = true;
            }
            ImGui::SameLine(); if(DrawColorButton(2, spriteManager->currentSprite->multi2Color, "Multi 2", &hovered)) {
                if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    colorSelectorOpen = true;
                    color = &spriteManager->currentSprite->multi2Color;
                } else {
                    selectedColor = 2;
                }
            }
            if(hovered) {
                is_hovered = true;
            }
        }
        ImGui::SameLine(); if(DrawColorButton(3, (masking ? MASK_TRANSPARENT : spriteManager->currentSprite->characterColor), (masking ? "Transparent" : "Character"), &hovered)) {
            if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                colorSelectorOpen = true;
                color = &spriteManager->currentSprite->characterColor;
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
            if(colorSelector.show(&colorSelectorOpen, *color, &spriteManager->currentSprite->palette)) {
                *color = colorSelector.selectedColor;
                spriteManager->currentSprite->Invalidate();
                animation.UpdateAllFramesIfAnimated(spriteManager->currentSprite);
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
