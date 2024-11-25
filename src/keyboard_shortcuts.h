#pragma once
#include <iostream>
#include <algorithm>
#include "imgui.h"
#include "menubar.h"

typedef std::function<void(void)> ShortcutFn;

struct Shortcut {
    ImGuiKey keycode;
    ImGuiKey mod;
    ShortcutFn fn;
    Shortcut(ImGuiKey keycode, ImGuiKey mod, ShortcutFn fn) : keycode(keycode), mod(mod), fn(fn) {}
};

#define ImGuiKey_NoMod ImGuiKey_None

struct KeyboardShortcuts {
    std::vector<Shortcut> shortcuts;

    KeyboardShortcuts(MenuBar *menubar, ProjectSprites *projectSprites, SpriteManager *spriteManager) {
        shortcuts = {
            // File
            Shortcut(ImGuiKey_N, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_NewProject(); }),
            Shortcut(ImGuiKey_O, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_OpenProject(); }),
            Shortcut(ImGuiKey_S, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_SaveProject(); }),
            Shortcut(ImGuiKey_E, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_ExportToClipboard(); }),
            Shortcut(ImGuiKey_F4, ImGuiKey_ModAlt, [menubar]() { menubar->Action_ExitApp(); }),

            // Edit
            Shortcut(ImGuiKey_F9, ImGuiKey_NoMod,  [projectSprites]() { projectSprites->Action_MoveUp(); }),
            Shortcut(ImGuiKey_F10, ImGuiKey_NoMod, [projectSprites]() { projectSprites->Action_MoveDown(); }),
            Shortcut(ImGuiKey_D, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_CloneSprite(); }),

            // Flip
            Shortcut(ImGuiKey_RightArrow, ImGuiKey_ModCtrl, [projectSprites]() { projectSprites->Action_FlipImage_Horizontal(); }),
            Shortcut(ImGuiKey_UpArrow, ImGuiKey_ModCtrl, [projectSprites]() { projectSprites->Action_FlipImage_Vertical(); }),

            // Shift
            Shortcut(ImGuiKey_UpArrow, ImGuiKey_ModShift, [projectSprites, spriteManager]() { projectSprites->Action_ShiftImage_Up(spriteManager->shiftRollingAround); }),
            Shortcut(ImGuiKey_DownArrow, ImGuiKey_ModShift, [projectSprites, spriteManager]() { projectSprites->Action_ShiftImage_Down(spriteManager->shiftRollingAround); }),
            Shortcut(ImGuiKey_LeftArrow, ImGuiKey_ModShift, [projectSprites, spriteManager]() { projectSprites->Action_ShiftImage_Left(spriteManager->shiftRollingAround); }),
            Shortcut(ImGuiKey_RightArrow, ImGuiKey_ModShift,[projectSprites, spriteManager]() { projectSprites->Action_ShiftImage_Right(spriteManager->shiftRollingAround); }),

            // Rotate
            Shortcut(ImGuiKey_RightArrow, ImGuiKey_ModAlt, [projectSprites]() { projectSprites->Action_RotateImage_Clockwise(); }),
            Shortcut(ImGuiKey_LeftArrow, ImGuiKey_ModAlt, [projectSprites]() { projectSprites->Action_RotateImage_CounterClockwise(); }),

            Shortcut(ImGuiKey_Delete, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_DeleteSprite(); }),
        };
    }

    void Handle() {
        ImGuiIO &io = ImGui::GetIO();
        for(auto it = shortcuts.begin(); it != shortcuts.end(); ++it) {
            Shortcut &sc = *it;
            if(ImGui::IsKeyPressed(sc.keycode) && (sc.mod == ImGuiKey_NoMod || (sc.mod == ImGuiKey_ModCtrl && io.KeyCtrl) || (sc.mod == ImGuiKey_ModAlt && io.KeyAlt) || (sc.mod == ImGuiKey_ModShift && io.KeyShift))) {
                sc.fn();
                break;
            }
        }
    }
};