#pragma once
#include <algorithm>
#include "imgui.h"
#include "sprite_manager.h"
#include "util.h"
#include "imgui_filedialog.h"
#include "advanced_settings.h"
#include "rearrange_colors.h"
#include "capture.h"

struct ProjectSprites
{
    SpriteManager *spriteManager;
    StatusBar *statusbar;
    Project *project;

    int lastSelectedSpriteId = 0;
    bool openExportToDialog = false;
    ImFileDialogInfo fileDialogInfo = {
        .title = "Export Project To Source Code",
        .type = ImGuiFileDialogType_SaveFile,
        .flags = 0, //ImGuiFileDialogFlags_FileMustExist,
        .fileName = "",
        .directoryPath = std::filesystem::current_path()
    };
    int one = 1;
    bool showAdvancedSettings = false, showRearrangeColors = false;
    size_t selectedColors[4];
    bool captureVisible=false;
    AdvancedSettings advancedSettings;
    RearrangeColors rearrangeColors;
    Capture capture;

    ProjectSprites(SpriteManager *spriteManager, StatusBar *statusbar, Project *project)
        : spriteManager(spriteManager), statusbar(statusbar), project(project) {}

    void Action_MoveUp() {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->selectedSpriteId = spriteManager->MoveUp(spriteManager->selectedSpriteId);
        }
    }

    void Action_MoveDown() {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->selectedSpriteId = spriteManager->MoveDown(spriteManager->selectedSpriteId);
        }
    }

    void Action_ClearSprite(size_t color) {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->ClearSprite(spriteManager->selectedSpriteId, color);
        }
    }

    void Action_FlipImage_Horizontal() {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->FlipImage_Horizontal(spriteManager->selectedSpriteId);
        }
    }

    void Action_FlipImage_Vertical() {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->FlipImage_Vertical(spriteManager->selectedSpriteId);
        }
    }

    void Action_ShiftImage_Up(bool rotate) {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->ShiftImage_Up(spriteManager->selectedSpriteId, rotate);
        }
    }

    void Action_ShiftImage_Down(bool rotate) {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->ShiftImage_Down(spriteManager->selectedSpriteId, rotate);
        }
    }

    void Action_ShiftImage_Left(bool rotate) {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->ShiftImage_Left(spriteManager->selectedSpriteId, rotate);
        }
    }

    void Action_ShiftImage_Right(bool rotate) {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->ShiftImage_Right(spriteManager->selectedSpriteId, rotate);
        }
    }

    void Action_RotateImage_Clockwise() {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->RotateImage_Clockwise(spriteManager->selectedSpriteId);
        }
    }

    void Action_RotateImage_CounterClockwise() {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->RotateImage_CounterClockwise(spriteManager->selectedSpriteId);
        }
    }

    void Action_InsertRow(size_t row) {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->InsertRow(spriteManager->selectedSpriteId, row);
        }
    }

    void Action_RemoveRow(size_t row) {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->RemoveRow(spriteManager->selectedSpriteId, row);
        }
    }

    void Action_InsertColumn(size_t col) {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->InsertColumn(spriteManager->selectedSpriteId, col);
        }
    }

    void Action_RemoveColumn(size_t col) {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->RemoveColumn(spriteManager->selectedSpriteId, col);
        }
    }

    void Action_RearrangeColors() {
        if(spriteManager->selectedSpriteId != -1) {
            showRearrangeColors = true;
            selectedColors[0] = 0;
            selectedColors[1] = 1;
            selectedColors[2] = 2;
            selectedColors[3] = 3;
        }
    }

    void Action_Capture() {
        captureVisible = true;
    }

    void Action_Copy() {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->CopySprite(spriteManager->selectedSpriteId);
        }
    }

    void Action_Paste() {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->PasteSprite(spriteManager->selectedSpriteId);
        }
    }

    void Action_InsertSprite() {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->selectedSpriteId = spriteManager->InsertNewSprite(spriteManager->selectedSpriteId);
        }
    }

    void Action_DeleteSprite() {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->selectedSpriteId = spriteManager->RemoveSprite(spriteManager->selectedSpriteId);
        }
    }

    void Action_CloneSprite() {
        if(spriteManager->selectedSpriteId != -1) {
            spriteManager->selectedSpriteId = spriteManager->CloneSprite(spriteManager->selectedSpriteId);
        }
    }

    void render(SDL_Renderer *renderer)
    {
        ImGui::Begin("Sprite Project");
        if (ImGui::BeginTabBar("#spriteProjectTab", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Sprites"))
             {
                if(ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
                    if(ImGui::Shortcut(ImGuiKey_ModCtrl | ImGuiKey_C, ImGuiInputFlags_None)) { Action_Copy(); }
                    if(ImGui::Shortcut(ImGuiKey_ModCtrl | ImGuiKey_V, ImGuiInputFlags_None)) { Action_Paste(); }
                }
                switch (spriteManager->spriteListType)
                {
                case 0:
                    listView(renderer);
                    break;
                case 1:
                    iconView(renderer);
                    break;
                }

                if (ImGui::Button("Add New Sprite", ImVec2(150,25))) {
                    spriteManager->AddNewSprite();
                }
                ImGui::SameLine();
                if (ImGui::Button("Capture from Screenshot", ImVec2(200,25))) {
                    Action_Capture();
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Project Metadata and Settings"))
            {
                if (ImGui::BeginTable("#projectMetadata", 2, ImGuiTableFlags_NoBordersInBody))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Project Name");
                        ImGui::TableNextColumn(); ImGui::PushID(0); ImGui::InputText("", project->header.projectName, IM_ARRAYSIZE(project->header.projectName)); ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Comments");
                        ImGui::TableNextColumn(); ImGui::PushID(1); ImGui::SetNextItemWidth(-1); ImGui::InputTextMultiline("", project->header.projectComments, IM_ARRAYSIZE(project->header.projectComments)); ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Platform");
                        ImGui::TableNextColumn(); ImGui::PushID(2); ImGui::InputText("", project->header.projectPlatform, IM_ARRAYSIZE(project->header.projectPlatform)); ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Created On");
                        ImGui::TableNextColumn(); ImGui::TextUnformatted(project->header.createdOn);

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Length");
                        ImGui::TableNextColumn(); ImGui::Text("%lu bytes ($%04x)", (unsigned long)statusbar->totalBytes, (int)statusbar->totalBytes);

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TableNextColumn(); ImGui::PushID(3); if(ImGui::Checkbox("Automatically export source code on save", &project->header.autoExportSourceCode)) {} ImGui::PopID();

                        ImGui::BeginDisabled(!project->header.autoExportSourceCode);
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TableNextColumn(); ImGui::PushID(4); if(ImGui::Checkbox("Include comments and metadata in", &project->header.includeMetadata)) {} ImGui::PopID();
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TableNextColumn(); ImGui::PushID(5); if(ImGui::Checkbox("Only data (with no labels and constants)", &project->header.onlyData)) {} ImGui::PopID();

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn(); ImGui::TextUnformatted("Export to");
                            ImGui::TableNextColumn(); ImGui::PushID(6); ImGui::InputText("", project->header.exportTo, IM_ARRAYSIZE(project->header.exportTo)); ImGui::SameLine(); if(ImGui::Button("...")) {
                                openExportToDialog = true;
                                fileDialogInfo.fileName = std::string(project->header.exportTo);
                            } ImGui::PopID();
                        ImGui::EndDisabled();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Char Index");
                        ImGui::TableNextColumn(); ImGui::PushID(7); ImGui::SetNextItemWidth(100); ImGui::InputScalar("", ImGuiDataType_U8, &project->header.startCharIndex, &one, nullptr, "%d", 0); ImGui::PopID();
                        ImGui::SameLine(); ImGui::PushID(8); if(ImGui::Button("Advanced ...") && !ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
                            showAdvancedSettings = true;
                        } ImGui::PopID();
                    }
                    ImGui::EndTable();
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        if(advancedSettings.show(spriteManager, &showAdvancedSettings)) {
            //
        }

        if(spriteManager->selectedSpriteId != -1 && rearrangeColors.show(spriteManager->GetSprite(spriteManager->selectedSpriteId)->multicolorMode, selectedColors, &showRearrangeColors)) {
            spriteManager->RearrangeColors(spriteManager->selectedSpriteId, selectedColors);
        }

        if (ImGui::FileDialog(&openExportToDialog, &fileDialogInfo))
        {
            // Result path in: fileDialogInfo.resultPath
            strncpy(project->header.exportTo, std::filesystem::relative(fileDialogInfo.resultPath, std::filesystem::current_path()).string().c_str(), IM_ARRAYSIZE(project->header.exportTo));
        }

        if(captureVisible) {
            if(capture.show(spriteManager, &captureVisible)) {
                int spId = spriteManager->CloneSprite(&capture.capturedSprite, spriteManager->AddNewSprite());
                Sprite *sp = spriteManager->GetSprite(spId);
                strncpy(sp->description, vformat("Captured from Screenshot (%s)\nPosX=%d; PosY=%d", capture.screenCaptureFile,
                    (int)capture.cutter_x, (int)capture.cutter_y).c_str(), sizeof(sp->description));
            }
        }

        ImGui::End();
    }

    void listView(SDL_Renderer *renderer)
    {
        ImVec2 outer_size = ImVec2(0.0f, -30);
        if (ImGui::BeginTable("#list", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable, outer_size))
        {
            ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Details", ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(spriteManager->sprites.size());
            while (clipper.Step())
            {
                size_t charOffset = project->header.startCharIndex;
                // calculate char offset before clipping
                for (int row = 0; row < clipper.DisplayStart; row++) {
                    auto sprite = spriteManager->sprites[row];
                    charOffset += sprite->GetCharOffset(); //sprite->widthInBytes*((sprite->heightInPixels+7)>>3);
                }

                for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                {
                    auto sprite = spriteManager->sprites[row];

                    ImGui::PushID(sprite->ID);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    auto oldCursorY = ImGui::GetCursorPosY();
                    bool isSelected = spriteManager->selectedSpriteId == sprite->ID;

                    // NOTE: ImGuiSelectableFlags_NoHighlight is not an official imGui flag!
                    auto flags = ImGuiSelectableFlags_NoHighlight | ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick;
                    if (ImGui::Selectable("", isSelected, flags, ImVec2(0, 64.0f))) {
                        spriteManager->selectedSpriteId = sprite->ID;
                    }

                    if(!ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
                        // Attach sprite to the editor if we double clicked on sprite
                        if(isSelected && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                            spriteManager->AttachSprite(sprite);
                        } else {
                            // Detach sprite from the editor if we selected different sprite
                            if(spriteManager->selectedSpriteId != lastSelectedSpriteId) {
                                spriteManager->DetachSprite();
                                lastSelectedSpriteId = spriteManager->selectedSpriteId;
                            }
                        }
                    }

                    ImGui::SetCursorPosY(oldCursorY);

                    // FIXME: Magic numbers :) Still does not solve a small render issue, but it makes it a bit less frequent.
                    ImVec2 display_size = ImVec2(64.5f, 64.5f);
                    ImGui::Image((ImTextureID)sprite->GetTextureFixedSize(renderer, display_size), display_size);

                    ImGui::SameLine();
                    ImGui::TextUnformatted(sprite->spriteID);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%lu; Char: %lu ($%02X)", (unsigned long)row, (unsigned long)charOffset, (unsigned int)charOffset);

                    sprite->charOffset = charOffset;
                    sprite->charIndex = row;

                    charOffset += sprite->GetCharOffset(); // sprite->widthInBytes*((sprite->heightInPixels+7)>>3);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%lux%lu pixels,\nByte Order: %s\n%s\nAnimated: %s, Masked: %s", 
                        (unsigned long)sprite->widthInBytes<<3, 
                        (unsigned long)sprite->heightInPixels,
                        sprite->GetByteAlignment().c_str(), 
                        (sprite->multicolorMode ? "Multicolor" : "2 colors"), 
                        (sprite->animationAttached ? "Yes" : "No"),
                        (sprite->masked ? "Yes" : "No"));

                    ImGui::PopID();
                }
            }
            statusbar->totalBytes = GetTotalBytes();
            statusbar->spriteCount = spriteManager->sprites.size();

            PopupMenu();
            ImGui::EndTable();
        }
    }

    void PopupMenu() {
        if (ImGui::BeginPopupContextWindow("##spriteProjectPopup")) {
            if(ImGui::MenuItem("Insert Sprite")) {
                Action_InsertSprite();
            }
            if(ImGui::MenuItem("Clone Sprite", "Ctrl+d")) {
                Action_CloneSprite();
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Delete sprite ...", "Ctrl+Delete")) {
                Action_DeleteSprite();
            }
            ImGui::EndPopup();
        }
    }

    size_t GetTotalBytes() {
        size_t totalBytes = 0;
        for(auto it=spriteManager->sprites.begin(); it != spriteManager->sprites.end(); ++it) {
            totalBytes += (*it)->GetByteSize();
        }
        return totalBytes;
    }

    void iconView(SDL_Renderer *renderer) {
        ImVec2 outer_size = ImVec2(0.0f, -30);
        size_t possible_columns = std::max((size_t)1, (size_t)std::floor(ImGui::GetContentRegionAvail().x / 128.0f));
        if (ImGui::BeginTable("#icons", possible_columns, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame, outer_size)) {
            ImGui::TableNextRow();
            for (size_t i = 0; i < spriteManager->sprites.size();++i)
            {
                ImGui::TableNextColumn();

                auto sprite = spriteManager->sprites[i];

                bool isSelected = spriteManager->selectedSpriteId == sprite->ID;
                const char *sprite_id_label = sprite->spriteID;
                // FIXME: See below
                // ImVec2 label_size = ImGui::CalcTextSize(sprite_id_label);

                const auto oldCursorX = ImGui::GetCursorPosX();
                const auto oldCursorY = ImGui::GetCursorPosY();
                ImGui::PushID(sprite->ID);

                    // FIXME: Proper calculation of height for the wrapped texts
                    // float text_height = ImGui::CalcTextSize(sprite->spriteID.c_str()).y*2;

                    float text_height = ImGui::GetTextLineHeightWithSpacing()*2;

                    // NOTE: ImGuiSelectableFlags_NoHighlight is not an official imGui flag!
                    auto flags = ImGuiSelectableFlags_NoHighlight | ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_AllowDoubleClick;
                    if (ImGui::Selectable("", isSelected, flags, ImVec2(0, 64.0f + text_height)))
                    {
                        spriteManager->selectedSpriteId = sprite->ID;
                    }

                    // Attach sprite to the editor if we double clicked on sprite
                    if(isSelected && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        spriteManager->currentSprite = sprite;
                        statusbar->is_zoom_visible = true;
                        statusbar->zoomIndex = sprite->zoomIndex;
                    } else {
                        // Detach sprite from the editor if we selected different sprite
                        if(spriteManager->selectedSpriteId != lastSelectedSpriteId) {
                            spriteManager->currentSprite = nullptr;
                            lastSelectedSpriteId = spriteManager->selectedSpriteId;
                        }
                    }

                    ImGui::SetCursorPosY(oldCursorY);

                    const auto column_width = ImGui::GetColumnWidth();
                    // FIXME: Magic numbers :) Still does not solve a small render issue, but it makes it a bit less frequent.
                    ImVec2 display_size = ImVec2(64.5f, 64.5f);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((column_width - display_size.x) * 0.5f));

                    ImGui::Image((ImTextureID)sprite->GetTextureFixedSize(renderer, display_size), display_size);
                    ImGui::SetCursorPosX(oldCursorX);

                    // FIXME: Does not really work with wrapped texts
                    // ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((column_width - label_size.x) * 0.5f));
                    // ImGui::TextUnformatted(trucate_text(sprite_id_label, column_width).c_str());
                    ImGui::TextWrapped("%s", sprite_id_label);
                    ImGui::SetCursorPosX(oldCursorX);

                ImGui::PopID();
            }
            PopupMenu();
            ImGui::EndTable();
        }
    }
};
