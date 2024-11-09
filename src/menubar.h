#pragma once
#include <iostream>
#include "imgui.h"
#include "imgui_filedialog.h"
#include "sprite_manager.h"
#include "project_sprites.h"

extern bool done;
extern bool show_demo_window;

struct MenuBar
{
    SpriteManager *spriteManager;
    ProjectSprites *projectSprites;

    ImFileDialogInfo saveDialogInfo = {
        .title = "Save Project As ...",
        .type = ImGuiFileDialogType_SaveFile,
        .flags = 0, //ImGuiFileDialogFlags_FileMustExist,
        .fileName = "",
        .directoryPath = std::filesystem::current_path()
    };
    bool openSaveProjectDialog = false;

    ImFileDialogInfo openDialogInfo = {
        .title = "Open Project ...",
        .type = ImGuiFileDialogType_OpenFile,
        .flags = ImGuiFileDialogFlags_FileMustExist,
        .fileName = "",
        .directoryPath = std::filesystem::current_path()
    };
    bool openOpenProjectDialog = false;

    MenuBar(SpriteManager *spriteManager, ProjectSprites* projectSprites): spriteManager(spriteManager), projectSprites(projectSprites) {}

    // File
    void Action_NewProject() {
        spriteManager->NewProject();
        projectSprites->selectedSpriteId = -1;
    }

    void Action_OpenProject() {
        if(!ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
            openOpenProjectDialog = true;
            openDialogInfo.fileName = spriteManager->projectFile;
        }
    }

    void Action_OpenProjectAs(const char *filename) {
        if(!ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
            if(spriteManager->LoadProject(filename)) {
                projectSprites->lastSelectedSpriteId = projectSprites->selectedSpriteId = -1;
            }
        }
    }

    void Action_SaveAsProject() {
        if(!ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
            openSaveProjectDialog = true;
            saveDialogInfo.fileName = spriteManager->projectFile;
        }
    }

    void Action_SaveProject() {
        if(!spriteManager->projectFile.empty()) {
            spriteManager->SaveProject();
            return;
        }
        Action_SaveAsProject();
    }

    void Action_ExportToClipboard() {

    }

    void Action_ExitApp() {
        done = true;
    }

    void Action_DeleteSprite() {
        if(projectSprites->selectedSpriteId != -1) {
            auto nextID = spriteManager->NextSpriteID(projectSprites->selectedSpriteId);
            spriteManager->RemoveSprite(projectSprites->selectedSpriteId);
            projectSprites->selectedSpriteId = nextID;
        }
    }

    void Action_CloneSprite() {
        if(projectSprites->selectedSpriteId != -1) {
            projectSprites->selectedSpriteId = spriteManager->CloneSprite(projectSprites->selectedSpriteId);
        }
    }

    void render() {
        if(ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("New Project", "Ctrl+N")) {
                    Action_NewProject();
                }
                if(ImGui::MenuItem("Open Project...", "Ctrl+O")) {
                    Action_OpenProject();
                }
                if(ImGui::MenuItem("Save Project", "Ctrl+S")) {
                    Action_SaveProject();
                }
                if(ImGui::MenuItem("Save Project As...")) {
                    Action_SaveAsProject();
                }
                ImGui::Separator();
                if (ImGui::BeginMenu("Export as Source Code"))
                {
                    if(ImGui::MenuItem("Export to File")) {}
                    if(ImGui::MenuItem("Export to Clipboard", "Ctrl+E")) {
                        Action_ExportToClipboard();
                    }
                    if(ImGui::Checkbox("Export with Comments and Metadata", &spriteManager->exporWithComments)) {}
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                for(auto it=spriteManager->MRU.begin(); it!=spriteManager->MRU.end();++it) {
                    auto entry = *it;
                    ImGui::PushID(entry.c_str());
                    if(ImGui::MenuItem(shrink_string(entry, 30).c_str())) {
                        Action_OpenProjectAs(entry.c_str());
                    }
                    ImGui::SetItemTooltip("%s", entry.c_str());
                    ImGui::PopID();
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Exit", "Alt+F4")) {
                  Action_ExitApp();
                }
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Edit")) {
                if(ImGui::BeginMenu("Add New Sprite")) {
                    if(ImGui::MenuItem("Blank Sprite")) {
                        spriteManager->NewSprite();
                    }
                    if(ImGui::MenuItem("Capture Sprite from Screenshot...", "F6")) {}
                    ImGui::EndMenu();
                }
                #if 0
                if(ImGui::MenuItem("Paste")) {}
                #endif
                if(ImGui::MenuItem("Clone Sprite", "Ctrl+D")) {
                    Action_CloneSprite();
                }
                if(ImGui::BeginMenu("Reorder Sprite")) {
                    if(ImGui::MenuItem("Move Up", "F9")) {
                        projectSprites->Action_MoveUp();
                    }
                    if(ImGui::MenuItem("Move Down", "F10")) {
                        projectSprites->Action_MoveDown();
                    }
                    ImGui::EndMenu();
                }
                #if 0
                if(ImGui::BeginMenu("Recolor Sprite")) {
                    if(ImGui::MenuItem("Change Background Color")) {}
                    if(ImGui::MenuItem("Change Multi1 Color")) {}
                    if(ImGui::MenuItem("Change Multi2 Color")) {}
                    if(ImGui::MenuItem("Change Character Color")) {}
                    ImGui::EndMenu();
                }
                #endif
                ImGui::Separator();
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
                #if 0
                ImGui::Separator();
                if(ImGui::BeginMenu("Row")) {
                    if(ImGui::MenuItem("Inset Row")) {}
                    if(ImGui::MenuItem("Remove Row")) {}
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Column")) {
                    if(ImGui::MenuItem("Inset Column")) {}
                    if(ImGui::MenuItem("Remove Column")) {}
                    ImGui::EndMenu();
                }
                #endif
                ImGui::Separator();
                if(ImGui::MenuItem("Rearrange Color Pixels...")) {
                    projectSprites->Action_RearrangeColos();
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Delete Sprite", "Ctrl+Delete")) {
                    Action_DeleteSprite();
                }
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("View")) {
                if(ImGui::BeginMenu("Sprite List")) {
                    if(ImGui::RadioButton("Details", &spriteManager->spriteListType, 0)) {}
                    if(ImGui::RadioButton("Large Icons", &spriteManager->spriteListType, 1)) {}
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Help")) {
                if(ImGui::MenuItem("About...")) {}
                if(ImGui::MenuItem("Show Demo Window...")) {
                    show_demo_window = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();

            if (ImGui::FileDialog(&openSaveProjectDialog, &saveDialogInfo))
            {
                if(!spriteManager->SaveProjectAs(saveDialogInfo.resultPath)) {
                    std::cerr << "ERROR: Failed to save project!" << std::endl;
                }
            }

            if (ImGui::FileDialog(&openOpenProjectDialog, &openDialogInfo)) {
                if(!spriteManager->LoadProject(openDialogInfo.resultPath)) {
                    std::cerr << "ERROR: Failed to load project!" << std::endl;
                } else {
                    projectSprites->lastSelectedSpriteId = projectSprites->selectedSpriteId = -1;
                }
            }
        }
    }
};
