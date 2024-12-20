#pragma once

#include <imgui.h>
#include "util.h"

#define ColRGBA( r, g, b, a ) ImU32((a<<24)|(b<<16)|(g<<8)|(r))
struct ColorDef {
	ImU32 color;
	const char *name;
};

ColorDef c64pal[16] = {
	{ .color = ColRGBA(0,0,0,255)		, .name = "Black" },
	{ .color = ColRGBA(255,255,255,255)	, .name = "White" },
	{ .color = ColRGBA(136,57,50,255)	, .name = "Red" },
	{ .color = ColRGBA(103,182,189,255)	, .name = "Cyan" },
	{ .color = ColRGBA(139,63,150,255)	, .name = "Violet/Purple" },
	{ .color = ColRGBA(85,160,73,255)	, .name = "Green" },
	{ .color = ColRGBA(64,49,141,255)	, .name = "Blue" },
	{ .color = ColRGBA(191,206,114,255)	, .name = "Yellow" },
	{ .color = ColRGBA(139,84,41,255)	, .name = "Orange" },
	{ .color = ColRGBA(87,66,0,255)		, .name = "Brown" },
	{ .color = ColRGBA(184,105,98,255)	, .name = "Light Red" },
	{ .color = ColRGBA(80,80,80,255)	, .name = "Dark Grey" },
	{ .color = ColRGBA(120,120,120,255)	, .name = "Grey 2" },
	{ .color = ColRGBA(148,224,137,255)	, .name = "Light Green" },
	{ .color = ColRGBA(120,105,196,255)	, .name = "Light Blue" },
	{ .color = ColRGBA(159,159,159,255)	, .name = "Light Grey" }
};

ColorDef plus4pal[128] = {
	// from vice : yape pal
	{ .color = ColRGBA(0,0,0,255)		, .name = "Black 1"},
	{ .color = ColRGBA(39,39,39,255)	, .name = "White 1"},
	{ .color = ColRGBA(96,15,16,255)	, .name = "Red 1"},
	{ .color = ColRGBA(0,64,63,255)		, .name = "Cyan 1"},
	{ .color = ColRGBA(86,4,102,255)	, .name = "Purple 1"},
	{ .color = ColRGBA(0,75,0,255)		, .name = "Green 1"},
	{ .color = ColRGBA(26,26,140,255)	, .name = "Blue 1"},
	{ .color = ColRGBA(53,52,0,255)		, .name = "Yellow 1"},
	{ .color = ColRGBA(83,30,0,255)		, .name = "Orange 1"},
	{ .color = ColRGBA(71,40,0,255)		, .name = "Brown 1"},
	{ .color = ColRGBA(24,67,0,255)		, .name = "Dark Green 1"},
	{ .color = ColRGBA(97,8,52,255)		, .name = "Pink 1"},
	{ .color = ColRGBA(0,71,30,255)		, .name = "Blue Green 1"},
	{ .color = ColRGBA(4,41,122,255)	, .name = "Light Blue 1"},
	{ .color = ColRGBA(40,19,143,255)	, .name = "Dark Blue 1"},
	{ .color = ColRGBA(8,72,0,255)		, .name = "Light Green 1"},
	{ .color = ColRGBA(0,0,0,255)		, .name = "Black 2"},
	{ .color = ColRGBA(55,55,55,255)	, .name = "White 2"},
	{ .color = ColRGBA(111,30,31,255)	, .name = "Red 2"},
	{ .color = ColRGBA(0,79,78,255)		, .name = "Cyan 2"},
	{ .color = ColRGBA(101,19,117,255)	, .name = "Purple 2"},
	{ .color = ColRGBA(4,90,5,255)		, .name = "Green 2"},
	{ .color = ColRGBA(42,42,156,255)	, .name = "Blue 2"},
	{ .color = ColRGBA(68,68,0,255)		, .name = "Yellow 2"},
	{ .color = ColRGBA(99,46,0,255)		, .name = "Orange 2"},
	{ .color = ColRGBA(86,56,0,255)		, .name = "Brown 2"},
	{ .color = ColRGBA(40,82,0,255)		, .name = "Dark Green 2"},
	{ .color = ColRGBA(112,23,67,255)	, .name = "Pink 2"},
	{ .color = ColRGBA(0,86,46,255)		, .name = "Blue Green 2"},
	{ .color = ColRGBA(20,56,138,255)	, .name = "Light Blue 2"},
	{ .color = ColRGBA(56,34,158,255)	, .name = "Dark Blue 2"},
	{ .color = ColRGBA(23,88,0,255)		, .name = "Light Green 2"},
	{ .color = ColRGBA(0,0,0,255)		, .name = "Black 3"},
	{ .color = ColRGBA(67,67,67,255)	, .name = "White 3"},
	{ .color = ColRGBA(124,43,44,255)	, .name = "Red 3"},
	{ .color = ColRGBA(11,92,91,255)	, .name = "Cyan 3"},
	{ .color = ColRGBA(114,32,130,255)	, .name = "Purple 3"},
	{ .color = ColRGBA(17,103,17,255)	, .name = "Green 3"},
	{ .color = ColRGBA(54,55,168,255)	, .name = "Blue 3"},
	{ .color = ColRGBA(81,80,0,255)		, .name = "Yellow 3"},
	{ .color = ColRGBA(111,58,0,255)	, .name = "Orange 3"},
	{ .color = ColRGBA(99,68,0,255)		, .name = "Brown 3"},
	{ .color = ColRGBA(52,95,0,255)		, .name = "Dark Green 3"},
	{ .color = ColRGBA(125,36,80,255)	, .name = "Pink 3"},
	{ .color = ColRGBA(10,99,58,255)	, .name = "Blue Green 3"},
	{ .color = ColRGBA(33,69,150,255)	, .name = "Light Blue 3"},
	{ .color = ColRGBA(69,47,171,255)	, .name = "Dark Blue 3"},
	{ .color = ColRGBA(36,101,0,255)	, .name = "Light Green 3"},
	{ .color = ColRGBA(0,0,0,255)		, .name = "Black 4"},
	{ .color = ColRGBA(85,85,85,255)	, .name = "White 4"},
	{ .color = ColRGBA(141,60,61,255)	, .name = "Red 4"},
	{ .color = ColRGBA(28,109,108,255)	, .name = "Cyan 4"},
	{ .color = ColRGBA(131,49,147,255)	, .name = "Purple 4"},
	{ .color = ColRGBA(34,120,34,255)	, .name = "Green 4"},
	{ .color = ColRGBA(71,72,185,255)	, .name = "Blue 4"},
	{ .color = ColRGBA(98,97,0,255)		, .name = "Yellow 4"},
	{ .color = ColRGBA(128,75,17,255)	, .name = "Orange 4"},
	{ .color = ColRGBA(116,85,0,255)	, .name = "Brown 4"},
	{ .color = ColRGBA(69,112,0,255)	, .name = "Dark Green 4"},
	{ .color = ColRGBA(142,53,97,255)	, .name = "Pink 4"},
	{ .color = ColRGBA(27,116,75,255)	, .name = "Blue Green 4"},
	{ .color = ColRGBA(50,86,167,255)	, .name = "Light Blue 4"},
	{ .color = ColRGBA(86,64,188,255)	, .name = "Dark Blue 4"},
	{ .color = ColRGBA(53,118,0,255)	, .name = "Light Green 4"},
	{ .color = ColRGBA(0,0,0,255)		, .name = "Black 5"},
	{ .color = ColRGBA(121,121,121,255)	, .name = "White 5"},
	{ .color = ColRGBA(178,97,98,255)	, .name = "Red 5"},
	{ .color = ColRGBA(64,145,144,255)	, .name = "Cyan 5"},
	{ .color = ColRGBA(167,85,183,255)	, .name = "Purple 5"},
	{ .color = ColRGBA(70,157,71,255)	, .name = "Green 5"},
	{ .color = ColRGBA(108,108,222,255)	, .name = "Blue 5"},
	{ .color = ColRGBA(134,134,20,255)	, .name = "Yellow 5"},
	{ .color = ColRGBA(165,112,53,255)	, .name = "Orange 5"},
	{ .color = ColRGBA(153,122,34,255)	, .name = "Brown 5"},
	{ .color = ColRGBA(106,148,21,255)	, .name = "Dark Green 5"},
	{ .color = ColRGBA(179,89,134,255)	, .name = "Pink 5"},
	{ .color = ColRGBA(63,152,112,255)	, .name = "Blue Green 5"},
	{ .color = ColRGBA(86,123,204,255)	, .name = "Light Blue 5"},
	{ .color = ColRGBA(122,100,225,255)	, .name = "Dark Blue 5"},
	{ .color = ColRGBA(89,154,34,255)	, .name = "Light Green 5"},
	{ .color = ColRGBA(0,0,0,255)		, .name = "Black 6"},
	{ .color = ColRGBA(169,169,169,255)	, .name = "White 6"},
	{ .color = ColRGBA(225,144,145,255)	, .name = "Red 6"},
	{ .color = ColRGBA(112,193,192,255)	, .name = "Cyan 6"},
	{ .color = ColRGBA(215,133,231,255)	, .name = "Purple 6"},
	{ .color = ColRGBA(118,204,118,255)	, .name = "Green 6"},
	{ .color = ColRGBA(156,156,255,255)	, .name = "Blue 6"},
	{ .color = ColRGBA(182,182,68,255)	, .name = "Yellow 6"},
	{ .color = ColRGBA(213,160,101,255)	, .name = "Orange 6"},
	{ .color = ColRGBA(200,169,82,255)	, .name = "Brown 6"},
	{ .color = ColRGBA(154,196,69,255)	, .name = "Dark Green 6"},
	{ .color = ColRGBA(226,137,181,255)	, .name = "Pink 6"},
	{ .color = ColRGBA(111,200,160,255)	, .name = "Blue Green 6"},
	{ .color = ColRGBA(134,170,251,255)	, .name = "Light Blue 6"},
	{ .color = ColRGBA(170,148,255,255)	, .name = "Dark Blue 6"},
	{ .color = ColRGBA(137,202,82,255)	, .name = "Light Green 6"},
	{ .color = ColRGBA(0,0,0,255)		, .name = "Black 7"},
	{ .color = ColRGBA(199,199,199,255)	, .name = "White 7"},
	{ .color = ColRGBA(255,175,176,255)	, .name = "Red 7"},
	{ .color = ColRGBA(143,224,223,255)	, .name = "Cyan 7"},
	{ .color = ColRGBA(246,163,255,255)	, .name = "Purple 7"},
	{ .color = ColRGBA(148,235,149,255)	, .name = "Green 7"},
	{ .color = ColRGBA(186,186,255,255)	, .name = "Blue 7"},
	{ .color = ColRGBA(212,212,98,255)	, .name = "Yellow 7"},
	{ .color = ColRGBA(243,190,131,255)	, .name = "Orange 7"},
	{ .color = ColRGBA(231,200,112,255)	, .name = "Brown 7"},
	{ .color = ColRGBA(184,226,99,255)	, .name = "Dark Green 7"},
	{ .color = ColRGBA(255,167,212,255)	, .name = "Pink 7"},
	{ .color = ColRGBA(141,231,190,255)	, .name = "Blue Green 7"},
	{ .color = ColRGBA(164,201,255,255)	, .name = "Light Blue 7"},
	{ .color = ColRGBA(200,179,255,255)	, .name = "Dark Blue 7"},
	{ .color = ColRGBA(168,232,112,255)	, .name = "Light Green 7"},
	{ .color = ColRGBA(0,0,0,255)		, .name = "Black 8"},
	{ .color = ColRGBA(250,250,250,255)	, .name = "White 8"},
	{ .color = ColRGBA(255,226,227,255)	, .name = "Red 8"},
	{ .color = ColRGBA(194,255,255,255)	, .name = "Cyan 8"},
	{ .color = ColRGBA(255,214,255,255)	, .name = "Purple 8"},
	{ .color = ColRGBA(199,255,200,255)	, .name = "Green 8"},
	{ .color = ColRGBA(237,237,255,255)	, .name = "Blue 8"},
	{ .color = ColRGBA(255,255,149,255)	, .name = "Yellow 8"},
	{ .color = ColRGBA(255,241,182,255)	, .name = "Orange 8"},
	{ .color = ColRGBA(255,251,163,255)	, .name = "Brown 8"},
	{ .color = ColRGBA(235,255,150,255)	, .name = "Dark Green 8"},
	{ .color = ColRGBA(255,218,255,255)	, .name = "Pink 8"},
	{ .color = ColRGBA(192,255,241,255)	, .name = "Blue Green 8"},
	{ .color = ColRGBA(215,252,255,255)	, .name = "Light Blue 8"},
	{ .color = ColRGBA(251,230,255,255)	, .name = "Dark Blue 8"},
	{ .color = ColRGBA(219,255,163,255)	, .name = "Light Green 8"},
};

struct ColorSelector
{
	const char *TITLE = "Pick a Palette Color";

    const char* paletteValues[2] = {
        "VIC - Commodore 64 (16 colors)",
        "TED - Commodore 264 Series (120 colors)",
    };
	ImU32 selectedColor;

    ColorSelector() {}

    bool show(bool *open, ImU32 current_color, Sprite::PaletteType *palette) {
        if(!*open)
            return false;

        bool completed = false;
		size_t selected = 128;
		switch(*palette) {
			case Sprite::PaletteType::C64_Pal:
			for(size_t n=0;n<16;++n) {
				if(c64pal[n].color == current_color) {
					selected = n;
					break;
				}
			}
			break;
			case Sprite::PaletteType::C264_Pal:
			for(size_t n=0;n<128;++n) {
				if(plus4pal[n].color == current_color) {
					selected = n;
					break;
				}
			}
			break;
		}

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::SetNextWindowSize(ImVec2(380.0f,310.0f));
		ImGui::OpenPopup(TITLE, ImGuiPopupFlags_None);
        if(ImGui::BeginPopupModal(TITLE, open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_Popup)) {
            ImGui::SetNextItemWidth(200.0f); ImGui::TextUnformatted("Palette");
            ImGui::SameLine(); ImGui::PushID(0); ImGui::SetNextItemWidth(-1); if(ImGui::BeginCombo("", paletteValues[(size_t)*palette])) {
                for (int n = 0; n < IM_ARRAYSIZE(paletteValues); n++) {
                    const bool is_selected = (*palette == (Sprite::PaletteType)n);
                    if (ImGui::Selectable(paletteValues[n], is_selected)) {
                        *palette = (Sprite::PaletteType)n;
                    }

                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            } ImGui::PopID();

            bool is_hover = false;
			int color_index;
			ColorDef* hovered_color;
            switch(*palette) {
                case Sprite::PaletteType::C64_Pal:
					// Remove all padding values when rendering color selector buttons
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
                    for(size_t n=0; n<16; ++n) {
						ImGui::PushID(n); if(n>=1) ImGui::SameLine();
						bool hovered;
						if(ColorSelectorButton(c64pal[n].color, n == selected, &hovered)) {
                            completed = true;
                            *open = false;
						}
						ImGui::PopID();
						if(hovered) {
							is_hover = true;
							hovered_color = &c64pal[n];
							color_index = n;
						}
                    }
					ImGui::PopStyleVar();
                break;
                case Sprite::PaletteType::C264_Pal:
                    size_t n = 0;
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
                    for(size_t y=0; y<8; ++y) {
                        for(size_t x=0; x<16; ++x, n++) {
							ImGui::PushID(n); if(x>=1) ImGui::SameLine();
							bool hovered;
							if(ColorSelectorButton(plus4pal[n].color, n == selected, &hovered)) {
								completed = true;
								*open = false;
							}
							ImGui::PopID();
							if(hovered) {
								is_hover = true;
								hovered_color = &plus4pal[n];
								color_index = y<<4|x;
							}
                        }
                    }
					ImGui::PopStyleVar();
				break;
            }

            if(is_hover) {
				selectedColor = hovered_color->color; // ABGR_BLUE(hovered_color->color)<<16|ABGR_GREEN(hovered_color->color)<<8|ABGR_RED(hovered_color->color);

                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 80.0f);
                ImGui::Text("$%02X %s (RGB: %d,%d,%d #%06x)",
					color_index,
					hovered_color->name,
					ABGR_RED(hovered_color->color),
					ABGR_GREEN(hovered_color->color),
					ABGR_BLUE(hovered_color->color),
					((ABGR_RED(hovered_color->color)<<16)|((ABGR_GREEN(hovered_color->color)<<8))|(ABGR_BLUE(hovered_color->color))));
            }

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 115.0f);
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30.0f);

            ImGui::PushID(1); if(ImGui::Button("Close", ImVec2(100,20))) {
                *open = false;
            } ImGui::PopID();
			ImGui::EndPopup();
        }

        return completed;
    }

	bool ColorSelectorButton(ImU32 color, bool selected, bool *hovered)
	{
		const float size  = 22.0f;
		const ImVec2 p = ImGui::GetCursorScreenPos();
		const ImVec2 button_size = ImVec2(size, size);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();
		bool pressed = ImGui::InvisibleButton("#color_button", button_size);

		draw_list->AddRectFilled(p, ImVec2(p.x + button_size.x, p.y + button_size.y), color);
		if(selected) {
			ImU32 comp_color = is_light_color(color) ? 0xff000000 : 0xffffffff;
			draw_list->AddLine(p, ImVec2(p.x + size, p.y + size), comp_color, 3.0f);
			draw_list->AddLine(ImVec2(p.x + size, p.y), ImVec2(p.x, p.y + size), comp_color, 3.0f);
		}
		bool is_hovered = ImGui::IsItemHovered();
		if(is_hovered) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		*hovered = is_hovered;
		return pressed;
	}
};
