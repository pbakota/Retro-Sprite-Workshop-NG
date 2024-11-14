#pragma once
#include <bits/stdc++.h>
#include "imgui.h"
#include "imgui_filedialog.h"
#include "imgui_clamped_window.h"
#include "util.h"
#include "sprite_manager.h"

extern SDL_Renderer *renderer;

struct Capture {
    SpriteManager *spriteManager;

    const char* TITLE = "Capture Sprite from Screenshot - %s";
    std::string title;
    char screenCaptureFile[512]={0};
    int gridLines = 3; // auto
    int zoomIndex = 0; // 100%
    bool alignOnePixel = false;
    float position_x, position_y;
    bool position_visible = false;
    bool cutter_dropped = false, cutter_drag = false;
    float cutter_x, cutter_y;
    ImVec2 imageViewRegionAvail;

    size_t pBorderTop;
    size_t pBorderLeft;

    bool openSetIgnoredBorderSize = false;

    ImFileDialogInfo captureImageDialog = {
        .title = "Load Screenshot",
        .type = ImGuiFileDialogType_OpenFile,
        .flags = ImGuiFileDialogFlags_FileMustExist,
        .fileName = "",
        .directoryPath = std::filesystem::current_path()
    };
    bool showCaptureImageDialog = false;
    bool captureSettingsMulticolor = false;
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
    size_t widthInBytes = 1, heightInPixels = 8;
    int one = 1;
    const char *paletteMapping[5] = {
        "Background",
        "Multi 1",
        "Multi 2",
        "Character",
        "- ignore -",
    };
    const char *zoomValues[11] = {
        /* 0*/"100%",
        /* 1*/"200%",
        /* 2*/"300%",
        /* 3*/"400%",
        /* 4*/"500%",
        /* 5*/"600%",
        /* 6*/"800%",
        /* 7*/"1000%",
        /* 8*/"1500%",
        /* 9*/"2000%",
        /*10*/"4000%",
    };
    const float zoomParams[11] = {
        /* 0*/ 1.00f,
        /* 1*/ 2.00f,
        /* 2*/ 3.00f,
        /* 3*/ 4.00f,
        /* 4*/ 5.00f,
        /* 5*/ 6.00f,
        /* 6*/ 8.00f,
        /* 7*/10.00f,
        /* 8*/10.50f,
        /* 9*/20.00f,
        /*10*/40.00f,
    };
    float scaleFactor = 1.0f;
    bool firstFrame = true;
    Uint32 capturedBitmap[4096];
    Sprite capturedSprite;
    struct CapturedColor {
        ImU32 color;
        int paletteIndex;
        CapturedColor(ImU32 c, int p): color(c), paletteIndex(p) {}
    };
    std::vector<CapturedColor> capturedColors;
    bool refresh_capture = false;

    Capture() {}

    bool show(SpriteManager *spriteManager, bool *open) {
        if(!*open) return false;

        bool completed = false;
        this->spriteManager = spriteManager;
        scaleFactor = zoomParams[zoomIndex];

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if(firstFrame) {
            ImGui::SetNextWindowSize(ImVec2(1200.0f, 600.0f));
            firstFrame = false;
        }

        title = vformat(TITLE, screenCaptureFile);
        ImGui::ClampingWindowToTheScreenEdges(title.c_str());

        ImGui::OpenPopup(title.c_str());
        if(ImGui::BeginPopupModal(title.c_str(), open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            Menubar(open);
            if(ImGui::BeginTable("##capture", 2)) {

                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 300.0f);

                ImGui::TableNextRow();
                if(spriteManager->captureScreenshot) {
                    CaptureNow();
                } else {
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("No image is currently loaded");
                    ImGui::Dummy(ImVec2(0,10.0f));
                    ImGui::TextUnformatted("Plase open an emulator screenshot (best effect with 320x200 pixels images)");
                    if(ImGui::Button("Open file from File")) {
                        showCaptureImageDialog = true;
                    }
                    ImGui::SameLine(); ImGui::BeginDisabled(); {
                        if(ImGui::Button("Open from Clipboard")) {}
                    } ImGui::EndDisabled();

                    ImGui::Dummy(ImVec2(-FLT_MIN,20.0f));
                    ImGui::TextUnformatted("Recently Opened Screenshots");
                    if (ImGui::BeginListBox("##recentList")) {
                        static int item_selected_idx = -1;
                        for (int n = 0; (size_t)n < spriteManager->captureMRU.size(); n++) {
                            const bool is_selected = (item_selected_idx == n);
                            ImGui::PushID(n); if (ImGui::Selectable(spriteManager->captureMRU[n].c_str(), is_selected)) {
                                item_selected_idx = n;
                            } ImGui::PopID();

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndListBox();

                        if(item_selected_idx != -1 && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                            LoadCapture(spriteManager->captureMRU[item_selected_idx]);
                        }
                    }
                }
                if(!spriteManager->captureScreenshot) ImGui::BeginDisabled();
                ImGui::TableNextColumn();
                ImGui::SeparatorText("Sprite Capture Settings");
                ImGui::TextUnformatted("Cutter Width "); ImGui::SameLine(110.0f); ImGui::PushID(1); if(ImGui::BeginCombo("", spriteWidthValues[widthInBytes-1])) {
                    for (size_t n = 0; n < IM_ARRAYSIZE(spriteWidthValues); n++) {
                        const bool is_selected = (widthInBytes == n+1);
                        if (ImGui::Selectable(spriteWidthValues[n], is_selected)) {
                            widthInBytes = n+1;
                            refresh_capture = true;
                        }
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                } ImGui::PopID();
                ImGui::TextUnformatted("Cutter Height");
                ImGui::SameLine(110.0f); ImGui::PushID(2); ImGui::SetNextItemWidth(100); if(ImGui::InputScalar("", ImGuiDataType_U8, &heightInPixels, &one, nullptr, "%d", 0)) {
                    if(heightInPixels < 1) heightInPixels = 1;
                    if(heightInPixels > 64) heightInPixels = 64;
                    refresh_capture = true;
                } ImGui::PopID();
                ImGui::TextUnformatted("Multicolor"); ImGui::SameLine(110.0f); ImGui::PushID(3); if(ImGui::Checkbox("##multicolor", &captureSettingsMulticolor)) {
                    UpdateCapturedSpriteBitmap();
                } ImGui::PopID();

                ImGui::SeparatorText("Palette Mapping");
                if(ImGui::BeginChild("##paletteMapping", ImVec2(-FLT_MIN, -250.0f))) {
                    int mapId = 0; for(auto it=capturedColors.begin(); it!=capturedColors.end(); ++it) {
                        CapturedColor &cc = *it;
                        if(PaletteMapping(mapId++, cc)) {
                            UpdateCapturedSpriteBitmap();
                        }
                    }
                    if(ImGui::Button("Ignore All", ImVec2(-FLT_MIN, 30.0f))) {
                        for(auto it=capturedColors.begin(); it!=capturedColors.end();++it) {
                            CapturedColor &color = *it;
                            color.paletteIndex = 4;
                        }
                        UpdateCapturedSpriteBitmap();
                    }
                    ImGui::EndChild();
                }

                ImGui::SeparatorText("Preview Captured Sprite");
                if(capturedSprite.widthInBytes != 0) {
                    // ImGui::Image((ImTextureID)capturedSprite.GetTexture(renderer, 4.0f), ImVec2(4.0f*(capturedSprite.widthInBytes<<3), 4.0f*capturedSprite.heightInPixels));
                    ImGui::Image((ImTextureID)capturedSprite.GetTextureFixedSize(renderer, ImVec2(128.0f,128.0f)), ImVec2(128.0f, 128.0f));
                }

                ImVec2 buttonSize(100,20);
                float widthNeeded = buttonSize.x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - widthNeeded - 100.0f);
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 60.0f);
                ImGui::PushID(5); if(ImGui::Button("Capture", buttonSize)) {
                    completed = true;
                } ImGui::PopID();

                if(!spriteManager->captureScreenshot) ImGui::EndDisabled();

                ImGui::EndTable();
            }

            if (ImGui::FileDialog(&showCaptureImageDialog, &captureImageDialog)) {
                LoadCapture(captureImageDialog.resultPath);
            }

            Statusbar();

            ImGui::EndPopup();
        }

        // Escape closes the dialog if this window is the top most window
        if(ImGui::IsKeyPressed(ImGuiKey_Escape) && ImGui::GetTopMostPopupModal()->ID == ImGui::FindWindowByName(title.c_str())->ID) {
            *open = false;
        }

        return completed;
    }

    void LoadCapture(const std::string &filename) {
        if(spriteManager->LoadCaptureImage(filename)) {
            strncpy(screenCaptureFile, std::filesystem::path(filename).filename().c_str(), sizeof(screenCaptureFile));
            CaptureColors(spriteManager->captureSurface);
        }
    }

    bool PaletteMapping(ImGuiID id, CapturedColor &cc) {
        bool changed = false;
        ImDrawList *draw = ImGui::GetWindowDrawList();
        ImGui::BeginGroup(); {
            ImVec2 p1 = ImGui::GetCursorScreenPos();
            ImGui::PushID(id); if(ImGui::BeginCombo("##palette", paletteMapping[cc.paletteIndex])) {
                for (int n = 0; n < IM_ARRAYSIZE(paletteMapping); n++) {
                    const bool is_selected = (cc.paletteIndex == n);
                    if (ImGui::Selectable(paletteMapping[n], is_selected)) {
                        cc.paletteIndex = n;
                        changed = true;
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            } ImGui::PopID();
            p1.x += 200.0f; p1.y += 1.0f; ImVec2 p2(p1.x+32.0f, p1.y+16.0f);
            draw->AddRectFilled(p1,p2,cc.color);
            draw->AddRect(p1,p2,ImGui::GetColorU32(ImGuiCol_Border));
            if(ImGui::IsMouseHoveringRect(p1,p2) && !ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
                ImGui::SetTooltip("$%06x", cc.color&0x00ffffff);
            }
         } ImGui::EndGroup();
        return changed;
    }

    void CaptureNow() {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        imageViewRegionAvail = ImGui::GetContentRegionAvail();
        if(ImGui::BeginChild("##image", ImVec2(-FLT_MIN, -30.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar)) {
            ImVec2 s = ImGui::GetCursorScreenPos();
            s.x += spriteManager->captureBorderLeft*scaleFactor;
            s.y += spriteManager->captureBorderTop*scaleFactor;

            ImVec2 size = ImVec2(spriteManager->captureScreenshotSize.x * scaleFactor, spriteManager->captureScreenshotSize.y * scaleFactor);

            ImVec2 uv0 = ImVec2(0,0);
            if(alignOnePixel) {
                // Normalized coordinates of pixel (1,0) in the captured image
                 uv0 = ImVec2(scaleFactor/size.x, 0);
                 size.x -= scaleFactor;
            }
            ImGui::Image((ImTextureID)spriteManager->captureScreenshot, size, uv0);

            DrawGrid(s);

            // Calculate position & move cutter
            float cutterWidth = (widthInBytes<<3), cutterHeight=heightInPixels;
            position_visible = false;
            bool is_mouse_hovering = (ImGui::IsMouseHoveringRect(s, ImVec2(s.x + size.x, s.y + size.y))) && !ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId);
            if(is_mouse_hovering) {
                const ImVec2 m = ImGui::GetMousePos();
                position_x = (m.x-s.x)/scaleFactor;
                position_y = (m.y-s.y)/scaleFactor;

                position_visible = true;

                if(!cutter_dropped) {
                    cutter_x = position_x-cutterWidth*0.5f;
                    cutter_y = position_y-cutterHeight*0.5f;
                    DrawCutter(s, ImVec2(cutter_x, cutter_y));
                    if(!cutter_drag && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                        cutter_dropped = true;
                        refresh_capture = true;
                    }
                }
            }
            ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = cutter_dropped;
            if(cutter_dropped) {
                DrawCutter(s, ImVec2(cutter_x, cutter_y));
                ImVec2 p = ImVec2(s.x + cutter_x*scaleFactor, s.y + cutter_y*scaleFactor);
                if((ImGui::IsMouseHoveringRect(ImVec2(p.x, p.y), ImVec2(p.x + cutterWidth*scaleFactor, p.y + cutterHeight*scaleFactor))) && !ImGui::IsPopupOpen((ImGuiID)0, ImGuiPopupFlags_AnyPopupId)) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                    ImGui::SetTooltip("Drag to relocate Cutter");
                    if(ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                        cutter_dropped = false;
                        cutter_drag = true;
                    }
                }
            }
            if(cutter_drag && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                cutter_dropped = true;
                cutter_drag = false;
                refresh_capture = true;
            }

            if(refresh_capture) {
                AnalyzeAndCapture();
                refresh_capture = false;
            }

            ImGui::EndChild();
        }
    }

    void DrawCutter(ImVec2 offset, ImVec2 pos) {
        ImDrawList *draw = ImGui::GetWindowDrawList();
        float cutterWidth = (widthInBytes<<3), cutterHeight=heightInPixels;

        // Rounding to pixel boundary
        float rx = pos.x*scaleFactor - std::fmod(pos.x*scaleFactor, scaleFactor), ry = pos.y*scaleFactor - std::fmod(pos.y*scaleFactor, scaleFactor);

        ImVec2 p1(offset.x + rx, offset.y + ry);
        ImVec2 p2(p1.x + cutterWidth*scaleFactor, p1.y + cutterHeight*scaleFactor);

        if(captureSettingsMulticolor) {
            for(size_t x=0;x<=cutterWidth;x+=2) {
                ImVec2 p3(p1.x + x*scaleFactor, p1.y),p4(p1.x+x*scaleFactor, p1.y+cutterHeight*scaleFactor);
                draw->AddLine(p3,p4,ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
            }
        } else {
            for(size_t x=0;x<=cutterWidth;++x) {
                ImVec2 p3(p1.x + x*scaleFactor, p1.y),p4(p1.x+x*scaleFactor, p1.y+cutterHeight*scaleFactor);
                draw->AddLine(p3,p4,ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
            }
        }
        for(size_t y=0;y<=cutterHeight;++y) {
            ImVec2 p3(p1.x, p1.y+y*scaleFactor),p4(p1.x+cutterWidth*scaleFactor, p1.y+y*scaleFactor);
            draw->AddLine(p3,p4,ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
        }
    }

    void DrawGrid(ImVec2 offset) {
        if(gridLines == 0) return;
        ImDrawList *draw = ImGui::GetWindowDrawList();

        const float cellSize = 8.0f;

        for(size_t y = 0; y<=spriteManager->captureScreenshotSize.y; y+=cellSize) {
            ImVec2 p1(offset.x, offset.y + y*scaleFactor);
            ImVec2 p2(offset.x+spriteManager->captureScreenshotSize.x*scaleFactor, offset.y + y * scaleFactor);
            draw->AddLine(p1,p2, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
        }
        for(size_t x = 0; x<=spriteManager->captureScreenshotSize.x; x+=cellSize) {
            ImVec2 p1(offset.x + x*scaleFactor, offset.y);
            ImVec2 p2(offset.x + x*scaleFactor, offset.y + spriteManager->captureScreenshotSize.y * scaleFactor);
            draw->AddLine(p1,p2, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
        }
    }

    void CaptureColors(SDL_Surface *image) {
        if(SDL_MUSTLOCK(image)) SDL_LockSurface(image);
        std::set<ImU32> colors;
        for(size_t y=0;y<spriteManager->captureScreenshotSize.y;++y) {
            for(size_t x=0;x<spriteManager->captureScreenshotSize.x;++x) {
                const Uint32 *pixel = (Uint32 *)((Uint8 *)image->pixels + y * image->pitch + x * image->format->BytesPerPixel);
                colors.insert(*pixel);
            }
        }
        capturedColors.clear();
        int index = 0;
        for(auto c=colors.begin(); c!=colors.end(); ++c, ++index) {
            capturedColors.emplace_back(CapturedColor(*c, index?4:0));
        }
        if(SDL_MUSTLOCK(image)) SDL_UnlockSurface(image);
    }

    void AnalyzeAndCapture() {
        if(SDL_MUSTLOCK(spriteManager->captureSurface)) SDL_LockSurface(spriteManager->captureSurface);

        // capture bitmap
        Uint32 *p = capturedBitmap;
        size_t widthInPixels = widthInBytes<<3;
        int cx = (int)cutter_x, cy=(int)cutter_y;
        // std::cerr << "cutter_x=" << cx << ", cutter_y=" << cy << std::endl;
        for(size_t y=cy;y<cy+heightInPixels;++y) {
            for(size_t x=cx;x<cx+widthInPixels;++x) {
                *p++ = *(Uint32 *)((Uint8 *)spriteManager->captureSurface->pixels + y * spriteManager->captureSurface->pitch + x * spriteManager->captureSurface->format->BytesPerPixel);
            }
        }

        if(SDL_MUSTLOCK(spriteManager->captureSurface)) SDL_UnlockSurface(spriteManager->captureSurface);

        // capture sprite
        capturedSprite.ClearData();
        capturedSprite.widthInBytes = widthInBytes;
        capturedSprite.heightInPixels = heightInPixels;
        capturedSprite.multicolorMode = captureSettingsMulticolor;
        capturedSprite.backgroundColor = 0xffffffff;
        capturedSprite.characterColor = 0xff000000;
        capturedSprite.multi1Color = 0xff00ff00;
        capturedSprite.multi2Color = 0xffffff00;

        UpdateCapturedSpriteBitmap();
    }

    void UpdateCapturedSpriteBitmap() {
        size_t widthInPixels = widthInBytes<<3;
        memset(capturedSprite.data, 0, sizeof(capturedSprite.data));
        for(size_t y=0;y<heightInPixels;++y) {
            if(captureSettingsMulticolor) {
                for(size_t x=0;x<widthInPixels;x+=2) {
                    Uint32 c1 = capturedBitmap[y*widthInPixels+x+0];
                    // Uint32 c2 = capturedBitmap[y*widthInPixels+x+1];
                    // auto it2 = std::find_if(capturedColors.begin(), capturedColors.end(), [c2](CapturedColor &s) { return s.color == c2; });
                    auto it1 = std::find_if(capturedColors.begin(), capturedColors.end(), [c1](CapturedColor &s) { return s.color == c1; });
                    CapturedColor color = *it1;
                    if(color.paletteIndex == 0) {
                        // std::cerr << "..";
                        capturedSprite.data[y*capturedSprite.pitch+x+0] = 0;
                        capturedSprite.data[y*capturedSprite.pitch+x+1] = 0;
                    } else if(color.paletteIndex == 1) {
                        // std::cerr << "AA";
                        capturedSprite.data[y*capturedSprite.pitch+x+0] = 0;
                        capturedSprite.data[y*capturedSprite.pitch+x+1] = 1;
                    } else if(color.paletteIndex == 2) {
                        // std::cerr << "**";
                        capturedSprite.data[y*capturedSprite.pitch+x+0] = 1;
                        capturedSprite.data[y*capturedSprite.pitch+x+1] = 0;
                    } else if(color.paletteIndex == 3) {
                        // std::cerr << "--";
                        capturedSprite.data[y*capturedSprite.pitch+x+0] = 1;
                        capturedSprite.data[y*capturedSprite.pitch+x+1] = 1;
                    } else {
                        // std::cerr << "xx";
                    }
                }
                // std::cerr << std::endl;
            } else {
                for(size_t x=0;x<widthInPixels;++x) {
                    Uint32 c = capturedBitmap[y*widthInPixels+x];
                    auto it = std::find_if(capturedColors.begin(), capturedColors.end(), [c](CapturedColor &s) { return s.color == c; });
                    CapturedColor color = *it;
                    if(color.paletteIndex == 3) capturedSprite.data[y*capturedSprite.pitch+x] = 1; else
                    if(color.paletteIndex == 0) capturedSprite.data[y*capturedSprite.pitch+x] = 0;
                }
            }
        }

        #if 0
        for(auto it=capturedColors.begin(); it!=capturedColors.end(); ++it) {
            CapturedColor &color = *it;
            std::cerr << "colorIndex=" << color.paletteIndex << ", color=" << vformat("0x%06x", color.color&0xffffff) << std::endl;
        }
        #endif

        // Assign colors
        auto color00 = std::find_if(capturedColors.begin(), capturedColors.end(), [](CapturedColor &s) { return s.paletteIndex == 0; });
        auto color01 = std::find_if(capturedColors.begin(), capturedColors.end(), [](CapturedColor &s) { return s.paletteIndex == 1; });
        auto color10 = std::find_if(capturedColors.begin(), capturedColors.end(), [](CapturedColor &s) { return s.paletteIndex == 2; });
        auto color11 = std::find_if(capturedColors.begin(), capturedColors.end(), [](CapturedColor &s) { return s.paletteIndex == 3; });
        if(color00 != capturedColors.end()) capturedSprite.backgroundColor = (*color00).color;
        if(color01 != capturedColors.end()) capturedSprite.multi1Color = (*color01).color;
        if(color10 != capturedColors.end()) capturedSprite.multi2Color = (*color10).color;
        if(color11 != capturedColors.end()) capturedSprite.characterColor = (*color11).color;

        capturedSprite.multicolorMode = captureSettingsMulticolor;

        #if 0
        std::cerr << vformat("backgroundColor =0x%06x", capturedSprite.backgroundColor) << std::endl;
        std::cerr << vformat("multi1Color     =0x%06x", capturedSprite.multi1Color) << std::endl;
        std::cerr << vformat("multi2Color     =0x%06x", capturedSprite.multi2Color) << std::endl;
        std::cerr << vformat("characterColor  =0x%06x", capturedSprite.characterColor) << std::endl;
        #endif

        capturedSprite.Invalidate();
    }

    void Statusbar() {
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 35.0f);
        ImGui::Separator();
        if(ImGui::BeginChild("##statusbar", ImVec2(0,0), ImGuiChildFlags_None)) {
            ImGui::BeginDisabled(spriteManager->captureSurface == nullptr);

            ImGui::SetNextItemWidth(600.0f);
            ImGui::AlignTextToFramePadding();
            if(position_visible) {
                ImGui::Text("PosX=%3d; PosY=%3d; CutterX=%3d; CutterY=%3d; CutterCharX=%2d; CutterCharY=%2d",
                    (int)position_x, (int)position_y, (int)cutter_x, (int)cutter_y, (int)(cutter_x/8),(int)(cutter_y/8));
            } else {
                ImGui::Text("CutterX=%2d; CutterY=%2d; CutterCharX=%2d; CutterCharY=%2d", (int)cutter_x, (int)cutter_y, (int)(cutter_x/8),(int)(cutter_y/8));
            }

            ImGui::SameLine(600.0f); ImGui::TextUnformatted("| Zoom:"); ImGui::PushID(1); ImGui::SameLine(); ImGui::SetNextItemWidth(150.0f); ImGui::AlignTextToFramePadding(); if(ImGui::BeginCombo("", zoomValues[zoomIndex])) {
                for (int n = 0; n < IM_ARRAYSIZE(zoomValues); n++) {
                    const bool is_selected = (zoomIndex == n);
                    if (ImGui::Selectable(zoomValues[n], is_selected)) {
                        zoomIndex = n;
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::Separator(); if(ImGui::Button("Zoom to Fit", ImVec2(-FLT_MIN, 20))) {
                    float w = (imageViewRegionAvail.x/spriteManager->captureScreenshotSize.x)-50.0f;
                    float h = (imageViewRegionAvail.y/spriteManager->captureScreenshotSize.y)-50.0f;

                    if(w>h) {
                        for(int i=IM_ARRAYSIZE(zoomParams)-1;i>=0;--i) {
                            float n = zoomParams[i]*spriteManager->captureScreenshotSize.y;
                            if(n<imageViewRegionAvail.y) {
                                zoomIndex = i;
                                break;
                            }
                        }
                    } else {
                        for(int i=IM_ARRAYSIZE(zoomParams)-1;i>=0;--i) {
                            float n = zoomParams[i]*spriteManager->captureScreenshotSize.x;
                            if(n<imageViewRegionAvail.x) {
                                zoomIndex = i;
                                break;
                            }
                        }
                    }
                }
                ImGui::EndCombo();
            } ImGui::PopID();

            ImGui::SameLine(); ImGui::AlignTextToFramePadding(); ImGui::Text("| Border Size: %d; %d", 0, 0);

            ImGui::EndDisabled();
            ImGui::EndChild();
        }
    }

    void Menubar(bool *open) {
        if(ImGui::BeginMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Open Screenshot from File...")) {
                    showCaptureImageDialog = true;
                }
                ImGui::Separator();
                for(auto it=spriteManager->captureMRU.begin(); it!=spriteManager->captureMRU.end();++it) {
                    auto entry = *it;
                    ImGui::PushID(entry.c_str());
                    if(ImGui::MenuItem(shrink_string(entry, 30).c_str())) {
                        LoadCapture(entry);
                    }
                    ImGui::SetItemTooltip("%s", entry.c_str());
                    ImGui::PopID();
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Exit Sprite Capture")) {
                    *open = false;
                }
                ImGui::EndMenu();
            }

            if(spriteManager->captureScreenshot) {
                if(ImGui::BeginMenu("Edit")) {
                    // if(ImGui::BeginMenu("Resize Capture Frame")) {
                    //     if(ImGui::MenuItem("Increase Width")) {}
                    //     if(ImGui::MenuItem("Decrease Width")) {}
                    //     if(ImGui::MenuItem("Increse Height")) {}
                    //     if(ImGui::MenuItem("Decrease Height")) {}
                    //     ImGui::EndMenu();
                    // }
                    if(ImGui::MenuItem("Align with one pixel", nullptr, &alignOnePixel)) {}
                    if(ImGui::MenuItem("Set Ignored Border Size...")) {
                        openSetIgnoredBorderSize = true;
                        pBorderLeft = spriteManager->captureBorderLeft;
                        pBorderTop = spriteManager->captureBorderTop;
                    }
                    ImGui::EndMenu();
                }

                if(ImGui::BeginMenu("View")) {
                    if(ImGui::BeginMenu("Grid Lines")) {
                        // if(ImGui::RadioButton("Dark", &gridLines, 1)) {}
                        // if(ImGui::RadioButton("Soft", &gridLines, 2)) {}
                        if(ImGui::RadioButton("Light", &gridLines, 3)) {}
                        // if(ImGui::RadioButton("Auto", &gridLines, 4)) {}
                        ImGui::Separator();
                        if(ImGui::RadioButton("None", &gridLines, 0)) {}
                        ImGui::EndMenu();
                    }
                    // if(ImGui::MenuItem("Scroll to Capture Frame", "F4")) {}
                    // if(ImGui::MenuItem("Match Cahracters from Sprite Set", "F5")) {}
                    ImGui::Separator();
                    if(ImGui::BeginMenu("Zoom")) {
                        if(ImGui::RadioButton("100%", &zoomIndex, 0)) {}
                        if(ImGui::RadioButton("500%", &zoomIndex, 4)) {}
                        if(ImGui::RadioButton("1000%", &zoomIndex, 7)) {}
                        if(ImGui::RadioButton("2000%", &zoomIndex, 9)) {}
                        if(ImGui::RadioButton("4000%", &zoomIndex, 10)) {}
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMenuBar();

            if(openSetIgnoredBorderSize) {
                if(SetIgnoredBorderSize(&openSetIgnoredBorderSize)) {
                    spriteManager->captureBorderTop =  pBorderTop;
                    spriteManager->captureBorderLeft = pBorderLeft;
                }
            }
        }
    }

    bool SetIgnoredBorderSize(bool *open) {
        if(!*open)
            return false;

        // NOTE: The return value is here opposite of the normal behavior
        // here we return true when the user cancelled the settings.
        bool cancel = false;
        const char* title = "Border Size";
        ImGuiStyle& style = ImGui::GetStyle();

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::SetNextWindowSize(ImVec2(400.0f,130.0f));
        ImGui::OpenPopup(title, ImGuiPopupFlags_None);
        if(ImGui::BeginPopupModal(title, open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            ImGui::TextUnformatted("Horizontal Border (margin on the top)");
            ImGui::SameLine(280.0f); ImGui::PushID(0); ImGui::SetNextItemWidth(100); if(ImGui::InputScalar("", ImGuiDataType_U8, &spriteManager->captureBorderTop, &one, nullptr, "%d", 0)) {
                if(spriteManager->captureBorderTop < 1) spriteManager->captureBorderTop = 0;
                if(spriteManager->captureBorderTop > 500) spriteManager->captureBorderTop = 500;
            } ImGui::PopID();
            ImGui::TextUnformatted("Vertical Border (margin on the left)");
            ImGui::SameLine(280.0f); ImGui::PushID(1); ImGui::SetNextItemWidth(100); if(ImGui::InputScalar("", ImGuiDataType_U8, &spriteManager->captureBorderLeft, &one, nullptr, "%d", 0)) {
                if(spriteManager->captureBorderLeft < 1) spriteManager->captureBorderLeft = 0;
                if(spriteManager->captureBorderLeft > 500) spriteManager->captureBorderLeft = 500;
            } ImGui::PopID();

            ImGui::Dummy(ImVec2(0, 20.0f));
            ImVec2 buttonSize(100,20);
            float widthNeeded = 3*buttonSize.x + 2*style.ItemSpacing.x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - widthNeeded);
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30.0f);
            ImGui::PushID(2); if(ImGui::Button("Reset", buttonSize)) {
                spriteManager->captureBorderLeft = 0;
                spriteManager->captureBorderTop = 0;
            } ImGui::PopID(); ImGui::SameLine();
            ImGui::PushID(3); if(ImGui::Button("OK", buttonSize)) {
                *open = false;
            } ImGui::PopID(); ImGui::SameLine();
            ImGui::PushID(4); if(ImGui::Button("Cancel", buttonSize)) {
                cancel = true;
                *open = false;
            } ImGui::PopID();
            ImGui::EndPopup();
        }

        return cancel;
    }
};