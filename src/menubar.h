#pragma once
#include <iostream>
#include "imgui.h"
#include "imgui_filedialog.h"
#include "sprite_manager.h"

extern bool done;
extern bool show_demo_window;

struct MenuBar
{
    SpriteManager *spriteManager;

    bool shiftRollingAround = true;
    int spriteListType = 0;
    bool exporWithComments = false;

    ImFileDialogInfo fileDialogInfo = {
        .title = "Save Project As ...",
        .type = ImGuiFileDialogType_SaveFile,
        .flags = 0, //ImGuiFileDialogFlags_FileMustExist,
        .fileName = "",
        .directoryPath = std::filesystem::current_path()
    };
    bool openSaveProjectDialog = false;

    MenuBar(SpriteManager *spriteManager): spriteManager(spriteManager) {}

    void render() {
        if(ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("New Project", "Ctrl+N")) {
                    spriteManager->NewProject();
                }
                if(ImGui::MenuItem("Open Project...", "Ctrl+O")) {}
                if(ImGui::MenuItem("Save Project", "Ctrl+S")) {}
                if(ImGui::MenuItem("Save Project As...")) {
                    openSaveProjectDialog = true;
                    fileDialogInfo.fileName = "project1.spr";
                }
                ImGui::Separator();
                if (ImGui::BeginMenu("Export as Source Code"))
                {
                    if(ImGui::MenuItem("Export to File")) {}
                    if(ImGui::MenuItem("Export to Clipboard", "Ctrl+E")) {}
                    if(ImGui::Checkbox("Export with Comments and Metadata", &exporWithComments)) {}
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if(ImGui::MenuItem("C:\\users\\sorel\\Deskt...ruce_lee_sprites.spr")) {}
                if(ImGui::MenuItem("C:\\users\\sorel\\Deskt...raidomos_sprite.spr")) {}
                ImGui::Separator();
                if(ImGui::MenuItem("Exit")) {
                  done = true;
                }
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Edit")) {
                if(ImGui::BeginMenu("Add New Sprite")) {
                    if(ImGui::MenuItem("Blank Sprite")) { }
                    if(ImGui::MenuItem("Capture Sprite from Screenshot...", "F6")) {}
                    ImGui::EndMenu();
                }
                if(ImGui::MenuItem("Paste")) {}
                if(ImGui::MenuItem("Clone Sprite")) {}
                if(ImGui::BeginMenu("Reorder Sprite")) {
                    if(ImGui::MenuItem("Move Up", "F9")) { }
                    if(ImGui::MenuItem("Move Down", "F10")) {}
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Recolor Sprite")) {
                    if(ImGui::MenuItem("Change Background Color")) {}
                    if(ImGui::MenuItem("Change Multi1 Color")) {}
                    if(ImGui::MenuItem("Change Multi2 Color")) {}
                    if(ImGui::MenuItem("Change Character Color")) {}
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if(ImGui::BeginMenu("Clear Image")) {
                    if(ImGui::MenuItem("Fill with Background Color")) {}
                    if(ImGui::MenuItem("Fill with Multi1 Color")) {}
                    if(ImGui::MenuItem("Fill with Multi2 Color")) {}
                    if(ImGui::MenuItem("Fill with Character Color")) {}
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Flip Image")) {
                    if(ImGui::MenuItem("Horizontally", "Ctrl+Shift+Right")) {}
                    if(ImGui::MenuItem("Vertically", "Ctrl+Shift+Down")) {}
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Shift Image")) {
                    if(ImGui::MenuItem("Up", "Ctrl+Alt+Up")) {}
                    if(ImGui::MenuItem("Down", "Ctrl+Alt+Down")) {}
                    if(ImGui::MenuItem("Left", "Ctrl+Alt+Left")) {}
                    if(ImGui::MenuItem("Right", "Ctrl+Alt+Right")) {}
                    ImGui::Separator();
                    if(ImGui::Checkbox("Rolling around", &shiftRollingAround)) {}
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Rotate Image")) {
                    if(ImGui::MenuItem("Clockwise")) {}
                    if(ImGui::MenuItem("Counter-clockwise")) {}
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Rearrange Color Pixels...")) {}
                if(ImGui::MenuItem("Delete Sprite")) {}
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("View")) {
                if(ImGui::BeginMenu("Sprite List")) {
                    if(ImGui::RadioButton("Details", &spriteListType, 0)) {}
                    if(ImGui::RadioButton("Large Icons", &spriteListType, 1)) {}
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

            if (ImGui::FileDialog(&openSaveProjectDialog, &fileDialogInfo))
            {
                // Result path in: fileDialogInfo.resultPath
                //strncpy(exportTo, std::filesystem::relative(fileDialogInfo.resultPath, std::filesystem::current_path()).c_str(), IM_ARRAYSIZE(exportTo));
                if(!spriteManager->SaveProject(fileDialogInfo.resultPath)) {
                    std::cerr << "ERROR: Failed to save project!" << std::ends;
                }
            }
        }
    }
};
