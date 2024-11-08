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

#define _S(a,b,c) Shortcut(a,b,c)
#define ImGuiKey_NoMod ImGuiKey_None

struct KeyboardShortcuts {
    std::vector<Shortcut> shortcuts;

    KeyboardShortcuts(MenuBar *menubar, ProjectSprites *projectSprites) {
        shortcuts = {
            // File
            _S(ImGuiKey_N, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_NewProject(); }),
            _S(ImGuiKey_O, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_OpenProject(); }),
            _S(ImGuiKey_S, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_SaveProject(); }),
            _S(ImGuiKey_E, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_ExportToClipboard(); }),
            _S(ImGuiKey_F4, ImGuiKey_ModAlt, [menubar]() { menubar->Action_ExitApp(); }),

            // Edit
            _S(ImGuiKey_F9, ImGuiKey_NoMod, [projectSprites]() { projectSprites->Action_MoveUp(); }),
            _S(ImGuiKey_F10, ImGuiKey_NoMod, [projectSprites]() { projectSprites->Action_MoveDown(); }),
            _S(ImGuiKey_D, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_CloneSprite(); }),
            _S(ImGuiKey_Delete, ImGuiKey_ModCtrl, [menubar]() { menubar->Action_DeleteSprite(); }),
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