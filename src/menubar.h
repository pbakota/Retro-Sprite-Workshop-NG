#pragma once
#include <iostream>
#include "imgui.h"
#include "imgui_filedialog.h"
#include "sprite_manager.h"
#include "project_sprites.h"
#include "project.h"
#include "generator.h"
#include "about.h"

extern bool exitApp;
extern bool wantExit;

#ifndef EXCLUDE_IMGUI_DEMO
extern bool show_demo_window;
#endif

struct MenuBar
{
    SpriteManager *spriteManager;
    ProjectSprites *projectSprites;
    Generator *generator;

    ImFileDialogInfo saveDialogInfo = {
        .title = "Save Project As",
        .type = ImGuiFileDialogType_SaveFile,
        .flags = 0, //ImGuiFileDialogFlags_FileMustExist,
        .fileName = "",
        .directoryPath = std::filesystem::current_path()
    };
    bool openSaveProjectDialog = false;

    ImFileDialogInfo openDialogInfo = {
        .title = "Open Project",
        .type = ImGuiFileDialogType_OpenFile,
        .flags = ImGuiFileDialogFlags_FileMustExist,
        .fileName = "",
        .directoryPath = std::filesystem::current_path()
    };
    bool openOpenProjectDialog = false;

    ImFileDialogInfo readDialogInfo = {
        .title = "Read Project",
        .type = ImGuiFileDialogType_OpenFile,
        .flags = ImGuiFileDialogFlags_FileMustExist,
        .fileName = "",
        .directoryPath = std::filesystem::current_path()
    };
    bool openReadProjectDialog = false;

    ImFileDialogInfo exportDialogInfo = {
        .title = "Export Project To Source Code As ...",
        .type = ImGuiFileDialogType_SaveFile,
        .flags = 0, //ImGuiFileDialogFlags_FileMustExist,
        .fileName = "",
        .directoryPath = std::filesystem::current_path()
    };
    bool openExportDialogInfo = false;

    bool showAboutDialog = false;
    About about;
    bool confirmNew = false, confirmOpen = false;
    std::string confirmOpenFilename;

    MenuBar(SpriteManager *spriteManager, ProjectSprites* projectSprites, Generator *generator)
        : spriteManager(spriteManager), projectSprites(projectSprites), generator(generator) {}

    // File
    void Action_NewProject() {
        if(spriteManager->projectUnsaved) {
            confirmNew = true;
        } else {
            spriteManager->NewProject();
            projectSprites->selectedSpriteId = -1;
        }
    }

    void Action_OpenProject() {
        if(!ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
            if(spriteManager->projectUnsaved) {
                confirmOpen = true;
                confirmOpenFilename.clear();
            } else {
                openOpenProjectDialog = true;
                openDialogInfo.fileName = spriteManager->projectFile;
            }
        }
    }

    void Action_OpenProjectAs(const char *filename) {
        if(!ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
            if(spriteManager->projectUnsaved) {
                confirmOpen = true;
                confirmOpenFilename = filename;
            } else {
                if(spriteManager->LoadProject(filename)) {
                    projectSprites->lastSelectedSpriteId = projectSprites->selectedSpriteId = -1;
                }
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
            if(spriteManager->project->autoExportSourceCode) {
                Action_ExportToFileTo();
            }
            return;
        }
        Action_SaveAsProject();
    }

	void Action_ReadProject() {
        if(!ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
           openReadProjectDialog = true;
           readDialogInfo.fileName = "";
        }		
	}

    void Action_ExportToClipboard() {
        std::string exported = generator->GenerateToString();
        SDL_SetClipboardText(exported.c_str());
    }

    void Action_ExportToFileTo() {
        if(generator->GenerateToFile(spriteManager->project->exportTo)) {
            //
        }
    }

    void Action_ExportToFileAs() {
        if(!ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
            openExportDialogInfo = true;
        }
    }

    void Action_ExitApp() {
        if(spriteManager->projectUnsaved) {
            wantExit = true;
        } else {
            exitApp = true;
        }
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
                if(ImGui::MenuItem("Read Project...")) {
                    Action_ReadProject();
                }                
                ImGui::Separator();
                if (ImGui::BeginMenu("Export as Source Code"))
                {
                    if(ImGui::MenuItem("Export to File")) {
                        Action_ExportToFileAs();
                    }
                    if(ImGui::MenuItem("Export to Clipboard", "Ctrl+E")) {
                        Action_ExportToClipboard();
                    }
                    if(ImGui::Checkbox("Export with Comments and Metadata", &spriteManager->exportWithComments)) {}
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
                if(ImGui::MenuItem("Copy", "Ctrl+c")) {
                    projectSprites->Action_Copy();
                }
                if(ImGui::MenuItem("Paste", "Ctrl+v")) {
                    projectSprites->Action_Paste();
                }
                ImGui::Separator();
                if(ImGui::BeginMenu("Add New Sprite")) {
                    if(ImGui::MenuItem("Blank Sprite")) {
                        spriteManager->NewSprite();
                    }
                    if(ImGui::MenuItem("Capture Sprite from Screenshot...", "F6")) {
                        projectSprites->Action_Capture();
                    }
                    ImGui::EndMenu();
                }
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
                ImGui::Separator();
                if(ImGui::MenuItem("Rearrange Color Pixels...")) {
                    projectSprites->Action_RearrangeColors();
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
                if(ImGui::Checkbox("Show Tiled View", &spriteManager->tiledWindowVisible)) {}
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Help")) {
                if(ImGui::MenuItem("About...")) {
                    showAboutDialog = true;
                }
                #ifndef EXCLUDE_IMGUI_DEMO
                if(ImGui::MenuItem("Show Demo Window...")) {
                    show_demo_window = true;
                }
                #endif
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();

            if (ImGui::FileDialog(&openSaveProjectDialog, &saveDialogInfo))
            {
                if(!spriteManager->SaveProjectAs(saveDialogInfo.resultPath.string().c_str())) {
                    std::cerr << "ERROR: Failed to save project!" << std::endl;
                } else {
                    if(spriteManager->project->autoExportSourceCode) {
                        Action_ExportToFileTo();
                    }
                }
            }

            if (ImGui::FileDialog(&openOpenProjectDialog, &openDialogInfo)) {
                if(!spriteManager->LoadProject(openDialogInfo.resultPath.string().c_str())) {
                    std::cerr << "ERROR: Failed to load project!" << std::endl;
                } else {
                    projectSprites->lastSelectedSpriteId = projectSprites->selectedSpriteId = -1;
                }
            }

            if (ImGui::FileDialog(&openReadProjectDialog, &readDialogInfo)) {
            	if(!spriteManager->ReadProject(readDialogInfo.resultPath.string().c_str())) {
            		std::cerr << "ERROR: Failed to read project!" << std::endl;
            	}
            }

            if (ImGui::FileDialog(&openExportDialogInfo, &exportDialogInfo)) {
                if(generator->GenerateToFile(exportDialogInfo.resultPath.string().c_str())) {
                    //
                }
            }

            if(showAboutDialog) {
               about.show(&showAboutDialog);
            }

            if(confirmNew) {
                if(ProjectNotSaved(&confirmNew, false)) {
                    spriteManager->NewProject();
                    projectSprites->selectedSpriteId = -1;
                }
            }

            if(confirmOpen) {
                if (ProjectNotSaved(&confirmOpen, false)) {
                    if (!confirmOpenFilename.empty()) {
                        if (spriteManager->LoadProject(confirmOpenFilename.c_str())) {
                            projectSprites->lastSelectedSpriteId = projectSprites->selectedSpriteId = -1;
                        }
                    } else {
                        openOpenProjectDialog = true;
                        openDialogInfo.fileName = spriteManager->projectFile;
                    }
                }
            }
        }
    }

    enum class ExitAction {
        Cancel,
        Yes,
        No
    };

    bool ProjectNotSaved(bool *open, bool exitFromApp) {
        ExitAction action;
        if(ProjectNotSavedDialog(open, exitFromApp, action)) {
            switch(action) {
                case ExitAction::Cancel: return false;
                case ExitAction::No: return true;
                case ExitAction::Yes:
                    if(!spriteManager->projectFile.empty()) {
                        spriteManager->SaveProject();
                        if(spriteManager->project->autoExportSourceCode) {
                            Action_ExportToFileTo();
                        }
                        return true;
                    }
                    openSaveProjectDialog = true;
                    saveDialogInfo.fileName = spriteManager->projectFile;
                    return false;
            }
        }

        return false;
    }

    bool ProjectNotSavedDialog(bool *open, bool exitFromApp, ExitAction &action) {
        if(!open) return false;

        bool completed = false;
        action = ExitAction::Cancel;

        ImGuiStyle& style = ImGui::GetStyle();

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        const char *title = "Confirm Save";

        ImGui::SetNextWindowSize(ImVec2(380.0f,120.0f));
        ImGui::OpenPopup(title, ImGuiPopupFlags_None);
        if(ImGui::BeginPopupModal(title, open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            const char *message1 = "There are unsaved changes in the project. Do you want to save them before closing?";
            const char *message2 = "There are unsaved changes in the project. Do you want to save them before opening another project?";
            ImGui::TextWrapped("%s", (exitFromApp ? message1 : message2));
            ImGui::Dummy(ImVec2(0, 20.0f));
            ImVec2 buttonSize(100,20);
            float widthNeeded = 3*buttonSize.x + 3*style.ItemSpacing.x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - widthNeeded);
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30.0f);
            ImGui::PushID(0); if(ImGui::Button("Yes", buttonSize)) {
                *open = false;
                action = ExitAction::Yes;
                completed = true;
            } ImGui::PopID();
            ImGui::SameLine(); ImGui::PushID(1); if(ImGui::Button("No", buttonSize)) {
                *open = false;
                action = ExitAction::No;
                completed = true;
            } ImGui::PopID();
            ImGui::SameLine(); ImGui::PushID(2); if(ImGui::Button("Cancel", buttonSize)) {
                *open = false;
                action = ExitAction::Cancel;
                completed = true;
            } ImGui::PopID();
            ImGui::EndPopup();
        }

        return completed;
    }
};
