#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct Sprite;
struct Project
{
    const char *CR = "\n";

    char projectName[128] = {0};
    char platformName[128] = {0};
    char projectComments[256] = {0};
    char createdOn[128] = {0};
    char exportTo[128] = {0};
    bool autoExportSourceCode = true;
    bool includeMetadata = true;

    Project() {}

    void NewProject() {
        memset((void*)projectName, 0, sizeof(projectName));
        memset((void*)platformName, 0, sizeof(platformName));
        memset((void*)projectComments, 0, sizeof(projectComments));
        memset((void*)createdOn, 0, sizeof(createdOn));
        memset((void*)exportTo, 0, sizeof(exportTo));
        strncpy(projectName, "Blank Project", sizeof(projectName));
        strncpy(createdOn, return_current_time_and_date().c_str(), sizeof(createdOn));
        strncpy(exportTo, "./export.int", sizeof(exportTo));
        autoExportSourceCode = false;
        includeMetadata = true;
    }

    bool Load(const std::string &filename, std::vector<Sprite*> &sprites) {
        sprites.clear();
        return false;
    }

    bool Save(const std::string &filename, std::vector<Sprite*> &sprites) {
        std::ofstream fs(filename, std::ios::trunc | std::ios::binary);
        if(!fs.is_open()) {
            return false;
        }

        fs << "# --- RETRO SPRITE WORKSHOP PROJECT FILE. V0.1.rev(140d3606) --- # rev(test) for debug\n";
        fs << "ProjectName=" << projectName << CR;
        fs << "Comments=" << projectComments << CR;
        fs << "Platform=" << platformName << CR;
        fs << "CreatedOn=" << createdOn << CR;
        fs << "AutomaticExportOnSave=" << autoExportSourceCode << CR;
        fs << "SourceCodeExportPath=" << exportTo << CR;
        fs << "AutomaticExportWithComments=" << includeMetadata << CR;

        int n = 1;
        for(auto it=sprites.begin(); it != sprites.end(); ++it, n++) {
            Sprite* sp = *it;
            fs << "Sprite" << n << ".ID=" << sp->spriteID << CR;
            fs << "Sprite" << n << ".Description=" << sp->description << CR;
            fs << "Sprite" << n << ".HeightInPixels=" << sp->heightInPixels << CR;
            fs << "Sprite" << n << ".WidthInBytes=" << sp->widthInBytes << CR;
            fs << "Sprite" << n << ".ByteAlignment=" << sp->GetByteAlignment() << CR;
            fs << "Sprite" << n << ".MulticolorMode=" << (sp->multicolorMode ? "True" : "False") << CR;
            fs << "Sprite" << n << ".BackgroundColor=" << "#BBBBBB\n";
            fs << "Sprite" << n << ".CharacterColor=" << "#EEEE77\n";
            fs << "Sprite" << n << ".Multi1Color=" << "#000000\n";
            fs << "Sprite" << n << ".Multi2Color=" << "#FFFFFF\n";
            fs << "Sprite" << n << ".Palette=" << "Commodore64\n";
            fs << "Sprite" << n << ".Data=" << "00000005171F1D1F0F0F3F3F3D350F0505040404040414540000000000000000000000000040C0C0C00000C0F1FDC04040501010101014150000000000000000\n";
            fs << "Sprite" << n << ".PrerenderSoftwareSprite=" << (sp->prerenderSoftwareSprite ? "True" : "False") << CR;
            fs << "Sprite" << n << ".RenderingPrecision=" << sp->GetRenderingPrecision() << CR;
            fs << "Sprite" << n << ".ZoomFactor=" << "1\n";
        }
        fs.close();
        return true;
    }
};