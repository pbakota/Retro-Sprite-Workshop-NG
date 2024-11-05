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

    typedef std::vector<std::pair<std::string, std::string>> keyvalue_list;

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

        keyvalue_list values;
        if(!ParseFile(filename, values)) {
            return false;
        }

        for(auto it = values.begin(); it!=values.end(); ++it) {
            const auto kv = *it;
            if(kv.first == "ProjectName") {
                strncpy(projectName, kv.second.c_str(), sizeof(projectName));
            } else if(kv.first == "Comments") {
                strncpy(projectComments, kv.second.c_str(), sizeof(projectComments));
            } else if(kv.first == "Platform") {
                strncpy(platformName, kv.second.c_str(), sizeof(platformName));
            } else if(kv.first == "CreatedOn") {
                strncpy(createdOn, kv.second.c_str(), sizeof(createdOn));
            } else if(kv.first == "AutomaticExportOnSave") {
                autoExportSourceCode = kv.second == "True";
            } else if (kv.first == "SourceCodeExportPath") {
               strncpy(exportTo, kv.second.c_str(), sizeof(exportTo));
            } else if(kv.first == "AutomaticExportWithComments") {
                includeMetadata = kv.second == "True";
            }
        }

        return true;
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
        fs << "AutomaticExportOnSave=" << (autoExportSourceCode ? "True" : "False") << CR;
        fs << "SourceCodeExportPath=" << exportTo << CR;
        fs << "AutomaticExportWithComments=" << (includeMetadata ? "True" : "False") << CR;

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
            fs << "Sprite" << n << ".ZoomFactor=" << sp->zoomIndex << CR;
        }
        fs.close();
        return true;
    }

    bool ParseFile(const std::string& filename, keyvalue_list &graph) {
        std::ifstream rd(filename, std::ios::binary);
        if(!rd.is_open()) {
            return false;
        }

        // read until you reach the end of the file
        for (std::string line; std::getline(rd, line); ) {

            std::string v = trim(line);
            if(v[0] == '#') continue; // skip comment

            std::size_t firstEqual = v.find('=');
            if(firstEqual==std::string::npos) continue; // skip invalid line

            std::string key = v.substr(0,firstEqual);
            std::string val = v.substr(firstEqual+1);

            graph.push_back(std::pair<std::string,std::string>(key, val));
        }

        rd.close();
        return true;
    }
};