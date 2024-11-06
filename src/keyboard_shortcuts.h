#pragma once
#include <iostream>
#include "imgui.h"

struct KeyboardShortcuts {

    KeyboardShortcuts() {}

    void Handle() {
        // if(!ImGui::IsWindowFocused()) return;

        if(ImGui::IsKeyPressed(ImGuiKey_X) && ImGui::IsKeyPressed(ImGuiKey_ModCtrl)) {
            std::cerr << "Exit from app" << std::endl;
        }
    }
};


