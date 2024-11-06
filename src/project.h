#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "sprite.h"

struct Project
{
    const char *CR = "\n";
    const char* PROJECT_FILE_SIGNATURE = "# --- RETRO SPRITE WORKSHOP --- ";
    const char* NEWLINE_PLACEHOLDER = "[{{NEWLINE}}]";

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

    // void ClearTempSpriteList(std::vector<Sprite*> &sprites) {
    //     for(auto it=sprites.rbegin(); it != sprites.rend(); ++it) {
    //         delete *it;
    //     }
    // }

    bool Load(const std::string &filename, std::vector<Sprite*> &sprites) {
        sprites.clear();

        keyvalue_list values;
        if(!ParseFile(filename, values)) {
            return false;
        }

        Sprite *sp;
        bool firstLine = true;
        for(auto it = values.begin(); it!=values.end(); ++it) {
            const auto kv = *it;

            if(firstLine) {
                // check file signature on first line
                if(kv.first != "#signature" && kv.second != PROJECT_FILE_SIGNATURE) {
                    // If the file is not a project file, report error
                    std::cerr << "ERROR: Invalid file signature!" << std::endl;
                    return false;
                }
                firstLine = false;
            }

            if(kv.first == "ProjectName") {
                strncpy(projectName, kv.second.c_str(), sizeof(projectName));
            } else if(kv.first == "Comments") {
                strncpy(projectComments, replace_string(kv.second, NEWLINE_PLACEHOLDER, "\n").c_str(), sizeof(projectComments));
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
            } else if(kv.first.substr(0,6) == "Sprite") {
                std::size_t firstDot = kv.first.find('.');
                if(firstDot==std::string::npos) continue; // skip invalid line
                std::string key = kv.first.substr(firstDot+1);
                if(key == "ID") {
                    sp = new Sprite(1,1, kv.second.c_str());
                    sprites.push_back(sp);
                } else if(key == "Description") {
                    strncpy(sp->description, replace_string(kv.second, NEWLINE_PLACEHOLDER, "\n").c_str(), sizeof(sp->description));
                } else if(key == "HeightInPixels") {
                    sp->heightInPixels = std::atoi(kv.second.c_str());
                } else if(key == "WidthInBytes") {
                    sp->widthInBytes = std::atoi(kv.second.c_str());
                } else if(key == "ByteAlignment") {
                    sp->byteAligment = sp->GetByteAlignment(kv.second);
                } else if(key == "MulticolorMode") {
                    sp->multicolorMode = kv.second == "True";
                } else if(key == "BackgroundColor") {
                    sp->backgroundColor = ReadRGBColor(kv.second)|0xff000000;
                } else if(key == "CharacterColor") {
                    sp->characterColor = ReadRGBColor(kv.second)|0xff000000;
                } else if(key == "Multi1Color") {
                    sp->multi1Color = ReadRGBColor(kv.second)|0xff000000;
                } else if(key == "Multi2Color") {
                    sp->multi2Color = ReadRGBColor(kv.second)|0xff000000;
                } else if(key == "Palette") {
                    sp->palette = sp->GetPaletteName(kv.second);
                } else if(key == "Data") {
                    HexDeserializeData(kv.second, sp->data, sp->widthInBytes, sp->heightInPixels, sp->pitch);
                } else if(key == "PrerenderSoftwareSprite") {
                    sp->prerenderSoftwareSprite = kv.second == "True";
                } else if(key == "RenderingPrecision") {
                    sp->renderingPrecision = sp->GetRenderingPrecision(kv.second);
                } else if(key == "ZoomFactor") {
                    // force zoom between 0 and 7
                    sp->zoomIndex = std::max(0, std::min(7, std::atoi(kv.second.c_str())));
                }
            }
        }

        return true;
    }

    bool Save(const std::string &filename, std::vector<Sprite*> &sprites) {
        std::ofstream fs(filename, std::ios::trunc | std::ios::binary);
        if(!fs.is_open()) {
            return false;
        }

        fs << PROJECT_FILE_SIGNATURE << CR;
        fs << "ProjectName=" << projectName << CR;
        fs << "Comments=" << replace_string(std::string(projectComments), "\n", NEWLINE_PLACEHOLDER) << CR;
        fs << "Platform=" << platformName << CR;
        fs << "CreatedOn=" << createdOn << CR;
        fs << "AutomaticExportOnSave=" << (autoExportSourceCode ? "True" : "False") << CR;
        fs << "SourceCodeExportPath=" << exportTo << CR;
        fs << "AutomaticExportWithComments=" << (includeMetadata ? "True" : "False") << CR;

        int n = 1;
        for(auto it=sprites.begin(); it != sprites.end(); ++it, n++) {
            Sprite* sp = *it;
            fs << "Sprite" << n << ".ID=" << sp->spriteID << CR;
            fs << "Sprite" << n << ".Description=" << replace_string(sp->description, "\n", NEWLINE_PLACEHOLDER) << CR;
            fs << "Sprite" << n << ".HeightInPixels=" << sp->heightInPixels << CR;
            fs << "Sprite" << n << ".WidthInBytes=" << sp->widthInBytes << CR;
            fs << "Sprite" << n << ".ByteAlignment=" << sp->GetByteAlignment() << CR;
            fs << "Sprite" << n << ".MulticolorMode=" << (sp->multicolorMode ? "True" : "False") << CR;
            fs << "Sprite" << n << ".BackgroundColor=" << WriteRGBColor(sp->backgroundColor) << CR;
            fs << "Sprite" << n << ".CharacterColor=" << WriteRGBColor(sp->characterColor) << CR;
            fs << "Sprite" << n << ".Multi1Color=" << WriteRGBColor(sp->multi1Color) << CR;
            fs << "Sprite" << n << ".Multi2Color=" << WriteRGBColor(sp->multi2Color) << CR;
            fs << "Sprite" << n << ".Palette=" << sp->GetPaletteName() << CR;
            fs << "Sprite" << n << ".Data=" << HexSerializeData(sp->data, sp->widthInBytes, sp->heightInPixels, sp->pitch) << CR;
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
        bool firstLine = true;
        for (std::string line; std::getline(rd, line); ) {

            std::string v = trim(line);
            if(v[0] == '#') {
                if(firstLine) {
                    graph.push_back(std::pair<std::string,std::string>("#signature", line));
                }
                continue; // skip comment
            }

            std::size_t firstEqual = v.find('=');
            if(firstEqual==std::string::npos) continue; // skip invalid line

            std::string key = v.substr(0,firstEqual);
            std::string val = v.substr(firstEqual+1);

            graph.push_back(std::pair<std::string,std::string>(key, val));
            firstLine = false;
        }

        rd.close();
        return true;
    }

    // NOTE: Width is in bytes
    std::string HexSerializeData(const char *dt, size_t widthInBytes, size_t heightInPixels, size_t pitch) {
        std::stringstream ss;
        size_t widthInPixels = widthInBytes<<3;
        for(size_t y=0; y<heightInPixels; ++y) {
            for(size_t x=0; x<widthInPixels; x+=8) {
                size_t b = 0;
                for(size_t p=0; p<8;++p) {
                    b = (b << 1) | dt[y*pitch+x+p];
                }
                ss << vformat("%02X", b);
            }
        }
        return ss.str();
    }

    void HexDeserializeData(const std::string &str, char *data, size_t widthInBytes, size_t heightInPixels, size_t pitch) {
        char temp[4096], *p = temp;
        memset((void*)p,0,sizeof(temp));
        for(auto it=str.begin(); it!=str.end(); it += 2) {
            size_t hi = *(it+0)-'0'; if(hi>=10) hi-=7;
            size_t lo = *(it+1)-'0'; if(lo>=10) lo-=7;
            *p++ = hi<<4|lo;
            // std::cerr << vformat("%02X", (size_t)(p[-1])&0xff) << std::endl;
        }


        size_t widthInPixels = widthInBytes<<3;
        p = temp;
        for(size_t y=0; y<heightInPixels; ++y) {
            for(size_t x=0; x<widthInPixels; x+=8, ++p) {
                // split bits into separate bytes
                for(size_t t=0, m = 128; t<8; ++t, m >>= 1) {
                    if(*p&m) data[y*pitch+x+t] = 1;
                }
            }
        }
    }

    ImU32 ReadRGBColor(const std::string& str) {
        if(str[0] != '#') return 0;
        ImU32 result = 0;
        for(auto it=str.begin()+1; it != str.end(); ++it) {
            size_t d = (*it)-'0'; if(d>=10) d-=7;
            result = result << 4 | d;
        }
        return result;
    }

    std::string WriteRGBColor(ImU32 color) {
        return vformat("#%02X%02X%02X", ABGR_RED(color), ABGR_GREEN(color), ABGR_BLUE(color));
    }
};