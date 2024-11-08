#pragma once
#include <algorithm>
#include "imgui.h"
#include "menubar.h"
#include "sprite_manager.h"
#include "sprite_image.h"
#include "util.h"
#include "imgui_filedialog.h"
#include "advanced_settings.h"

struct ProjectSprites
{
    SpriteManager *spriteManager;
    SpriteImage *spriteImage;
    StatusBar *statusbar;
    Project *project;

    int selectedSpriteId = 0;
    int lastSelectedSpriteId = 0;
    bool openExportToDialog = false;
    ImFileDialogInfo fileDialogInfo = {
        .title = "Export Project To Source Code",
        .type = ImGuiFileDialogType_SaveFile,
        .flags = 0, //ImGuiFileDialogFlags_FileMustExist,
        .fileName = "",
        .directoryPath = std::filesystem::current_path()
    };
    char startCharIndex = 0;
    int one = 1;
    bool showAdvancedSettings = false;
    AdvancedSettings advancedSettings;

    ProjectSprites(SpriteManager *spriteManager, SpriteImage *spriteImage, StatusBar *statusbar, Project *project)
        : spriteManager(spriteManager), spriteImage(spriteImage), statusbar(statusbar), project(project) {}


    void Action_MoveUp() {
        if(selectedSpriteId != -1) {
            selectedSpriteId = spriteManager->MoveUp(selectedSpriteId);
        }
    }

    void Action_MoveDown() {
        if(selectedSpriteId != -1) {
            selectedSpriteId = spriteManager->MoveDown(selectedSpriteId);
        }
    }

    void Action_ClearSprite(size_t color) {
        if(selectedSpriteId != -1) {
            spriteManager->ClearSprite(selectedSpriteId, color);
        }
    }

    void render(SDL_Renderer *renderer)
    {
        ImGui::Begin("Project Sprites");
        if (ImGui::BeginTabBar("#projectSprites", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Project Sprites"))
            {
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
                    spriteManager->NewSprite();
                }
                ImGui::SameLine();
                if (ImGui::Button("Capture from Screenshot", ImVec2(200,25)))
                {
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
                        ImGui::TableNextColumn(); ImGui::PushID(0); ImGui::InputText("", project->projectName, IM_ARRAYSIZE(project->projectName)); ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Comments");
                        ImGui::TableNextColumn(); ImGui::PushID(1); ImGui::SetNextItemWidth(-1); ImGui::InputTextMultiline("", project->projectComments, IM_ARRAYSIZE(project->projectComments)); ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Platform");
                        ImGui::TableNextColumn(); ImGui::PushID(2); ImGui::InputText("", project->platformName, IM_ARRAYSIZE(project->platformName)); ImGui::PopID();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Created On");
                        ImGui::TableNextColumn(); ImGui::TextUnformatted(project->createdOn);

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Length");
                        ImGui::TableNextColumn(); ImGui::Text("%lu bytes ($%04x)", statusbar->totalBytes, (int)statusbar->totalBytes);

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TableNextColumn(); ImGui::PushID(3); if(ImGui::Checkbox("Automatically export source code on save", &project->autoExportSourceCode)) {} ImGui::PopID();

                        ImGui::BeginDisabled(!project->autoExportSourceCode);
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TableNextColumn(); ImGui::PushID(4); if(ImGui::Checkbox("Include comments and metadata in", &project->includeMetadata)) {} ImGui::PopID();

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn(); ImGui::TextUnformatted("Export to");
                            ImGui::TableNextColumn(); ImGui::PushID(5); ImGui::InputText("", project->exportTo, IM_ARRAYSIZE(project->exportTo)); ImGui::SameLine(); if(ImGui::Button("...")) {
                                openExportToDialog = true;
                                fileDialogInfo.fileName = std::string(project->exportTo);
                            } ImGui::PopID();
                        ImGui::EndDisabled();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::TextUnformatted("Char Index");
                        ImGui::TableNextColumn(); ImGui::PushID(6); ImGui::SetNextItemWidth(100); ImGui::InputScalar("", ImGuiDataType_U8, &startCharIndex, &one, nullptr, "%d", 0); ImGui::PopID();
                        ImGui::SameLine(); ImGui::PushID(7); if(ImGui::Button("Advanced ...") && !ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
                            showAdvancedSettings = true;
                        } ImGui::PopID();
                    }
                    ImGui::EndTable();
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        if(advancedSettings.show(&showAdvancedSettings)) {
            // Result
        }

        if (ImGui::FileDialog(&openExportToDialog, &fileDialogInfo))
        {
            // Result path in: fileDialogInfo.resultPath
            strncpy(project->exportTo, std::filesystem::relative(fileDialogInfo.resultPath, std::filesystem::current_path()).c_str(), IM_ARRAYSIZE(project->exportTo));
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
                size_t charOffset = 0;
                // calculate char offset before clipping
                for (int row = 0; row < clipper.DisplayStart; row++) {
                    auto sprite = spriteManager->sprites[row];
                    charOffset += sprite->widthInBytes*((sprite->heightInPixels+7)>>3);
                }

                for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                {
                    auto sprite = spriteManager->sprites[row];

                    ImGui::PushID(sprite->ID);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    auto oldCursorY = ImGui::GetCursorPosY();
                    bool isSelected = selectedSpriteId == sprite->ID;

                    // NOTE: ImGuiSelectableFlags_NoHighlight is not an official imGui flag!
                    auto flags = ImGuiSelectableFlags_NoHighlight | ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick;
                    if (ImGui::Selectable("", isSelected, flags, ImVec2(0, 64.0f))) {
                        selectedSpriteId = sprite->ID;
                    }

                    if(!ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
                        // Attach sprite to the editor if we double clicked on sprite
                        if(isSelected && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                            spriteManager->AttachSprite(sprite);
                        } else {
                            // Detach sprite from the editor if we selected different sprite
                            if(selectedSpriteId != lastSelectedSpriteId) {
                                spriteManager->DetachSprite();
                                lastSelectedSpriteId = selectedSpriteId;
                            }
                        }
                    }

                    ImGui::SetCursorPosY(oldCursorY);

                    ImVec2 display_size = ImVec2(60.0f, 60.0f);
                    ImGui::Image((ImTextureID)sprite->GetTextureFixedSize(renderer, display_size), display_size);

                    ImGui::SameLine();
                    ImGui::TextUnformatted(sprite->spriteID);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d; Char: %lu", row, charOffset);

                    charOffset += sprite->widthInBytes*((sprite->heightInPixels+7)>>3);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%lux%lu pixels,\nByte Order: %s\n%s", sprite->widthInBytes<<3, sprite->heightInPixels,
                        sprite->GetByteAlignment().c_str(), sprite->multicolorMode ? "Multicolor" : "2 colors");

                    ImGui::PopID();
                }
            }
            statusbar->totalBytes = GetTotalBytes();
            statusbar->spriteCount = spriteManager->sprites.size();
            ImGui::EndTable();
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

                bool isSelected = selectedSpriteId == sprite->ID;
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
                        selectedSpriteId = sprite->ID;
                    }

                    // Attach sprite to the editor if we double clicked on sprite
                    if(isSelected && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        spriteImage->currentSprite = sprite;
                        statusbar->is_zoom_visible = true;
                        statusbar->zoomIndex = sprite->zoomIndex;
                    } else {
                        // Detach sprite from the editor if we selected different sprite
                        if(selectedSpriteId != lastSelectedSpriteId) {
                            spriteImage->currentSprite = nullptr;
                            lastSelectedSpriteId = selectedSpriteId;
                        }
                    }

                    ImGui::SetCursorPosY(oldCursorY);

                    const auto column_width = ImGui::GetColumnWidth();
                    ImVec2 display_size = ImVec2(60.0f, 60.0f);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((column_width - display_size.x) * 0.5f));
                    // ImGui::Image((ImTextureID)sprite->GetTexture(renderer, scale), size);

                    ImGui::Image((ImTextureID)sprite->GetTextureFixedSize(renderer, display_size), display_size);
                    ImGui::SetCursorPosX(oldCursorX);

                    // FIXME: Does not really work with wrapped texts
                    // ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((column_width - label_size.x) * 0.5f));
                    // ImGui::TextUnformatted(trucate_text(sprite_id_label, column_width).c_str());
                    ImGui::TextWrapped("%s", sprite_id_label);
                    ImGui::SetCursorPosX(oldCursorX);

                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }
};
