#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "sprite.h"
#include "serializator.h"

const char* PROJECT_FILE_SIGNATURE = "# --- RETRO SPRITE WORKSHOP --- ";
const char* NEWLINE_PLACEHOLDER = "[{{NEWLINE}}]";

extern SDL_Renderer *renderer;

struct Project
{
    struct ProjectHeader {
        char projectFile[512] = {0};
        char projectName[128] = {0};
        char projectPlatform[128] = {0};
        char projectComments[256] = {0};
        char createdOn[128] = {0};
        char exportTo[128] = {0};
        bool autoExportSourceCode = true;
        bool includeMetadata = true;
        bool onlyData = false;
        int startCharIndex = 0;
    } header;

    std::vector<float> zoomValues = {
        0.25f,
        0.50f,
        0.75f,
        1.00f,
        1.25f,
        1.50f,
        1.75f,
        2.00f,
    };

    typedef std::vector<std::pair<std::string, std::string>> keyvalue_list;

    Project() {}

    void NewProject() {
        memset((void*)header.projectName, 0, sizeof(header.projectName));
        memset((void*)header.projectPlatform, 0, sizeof(header.projectPlatform));
        memset((void*)header.projectComments, 0, sizeof(header.projectComments));
        memset((void*)header.createdOn, 0, sizeof(header.createdOn));
        memset((void*)header.exportTo, 0, sizeof(header.exportTo));
        strncpy(header.projectName, "Blank Project", sizeof(header.projectName));
        strncpy(header.createdOn, return_current_time_and_date().c_str(), sizeof(header.createdOn));
        strncpy(header.exportTo, "./export.inc", sizeof(header.exportTo));
        header.autoExportSourceCode = false;
        header.includeMetadata = true;
        header.onlyData = false;
        header.startCharIndex = 0;
    }

    // void ClearTempSpriteList(std::vector<Sprite*> &sprites) {
    //     for(auto it=sprites.rbegin(); it != sprites.rend(); ++it) {
    //         delete *it;
    //     }
    // }

    bool Load(const char *filename, ProjectHeader &hdr, std::vector<Sprite*> &sprites) {
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
                strncpy(hdr.projectName, kv.second.c_str(), sizeof(hdr.projectName));
            } else if(kv.first == "Comments") {
                strncpy(hdr.projectComments, replace_string(kv.second, NEWLINE_PLACEHOLDER, "\n").c_str(), sizeof(hdr.projectComments));
            } else if(kv.first == "Platform") {
                strncpy(hdr.projectPlatform, kv.second.c_str(), sizeof(hdr.projectPlatform));
            } else if(kv.first == "CreatedOn") {
                strncpy(hdr.createdOn, kv.second.c_str(), sizeof(hdr.createdOn));
            } else if(kv.first == "AutomaticExportOnSave") {
                hdr.autoExportSourceCode = kv.second == "True";
            } else if (kv.first == "SourceCodeExportPath") {
               strncpy(hdr.exportTo, kv.second.c_str(), sizeof(hdr.exportTo));
            } else if(kv.first == "AutomaticExportWithComments") {
                hdr.includeMetadata = kv.second == "True";
            } else if(kv.first == "OnlyData") {
                hdr.onlyData = kv.second == "True";
            } else if(kv.first == "StartCharIndex") {
                hdr.startCharIndex = std::atoi(kv.second.c_str());
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
                    Serializator::HexDeserializeData(kv.second, sp->data, sp->widthInBytes, sp->heightInPixels, sp->pitch);
                } else if(key == "PrerenderSoftwareSprite") {
                    sp->prerenderSoftwareSprite = kv.second == "True";
                } else if(key == "RenderingPrecision") {
                    sp->renderingPrecision = sp->GetRenderingPrecision(kv.second);
                } else if(key == "ZoomFactor") {
                    // force zoom between 0 and 7
                    //sp->zoomIndex = std::max(0, std::min(7, std::atoi(kv.second.c_str())));
                    auto index = std::atof(kv.second.c_str());
                    auto it = find_if(zoomValues.begin(), zoomValues.end(), [index](auto&&value){ return index == value; });
                    if(it == zoomValues.end()) {
                        sp->zoomIndex = 3;
                    } else {
                        sp->zoomIndex = it - zoomValues.begin();
                    }
                } else if(key == "AnimationAttached") {
                    sp->animationAttached = kv.second == "True";
                } else if(key == "AnimationFPS") {
                    sp->animationFPS = std::atoi(kv.second.c_str());
                } else if(key.substr(0,5) == "Frame") {
                    if(sp->animationAttached) {
                        auto &frame = sp->animationFrames.emplace_back();
                        frame.image = Sprite::CreateSpriteImageTexture(renderer);
                        Serializator::HexDeserializeData(kv.second.c_str(), frame.data, sp->widthInBytes, sp->heightInPixels, sp->pitch);
                        sp->UpdateTextureFromSpriteData(renderer, frame.image, frame.data);
                    }
                } else if(key.substr(0,5) == "Mask.") {
                    auto &frame = sp->animationFrames.back();
                    Serializator::HexDeserializeData(kv.second.c_str(), frame.mask, sp->widthInBytes, sp->heightInPixels, sp->pitch);
                } else if(key == "Masked") {
                    sp->masked = kv.second == "True";
                } else if(key == "Mask") {
                    if(sp->masked) {
                      Serializator::HexDeserializeData(kv.second, sp->mask, sp->widthInBytes, sp->heightInPixels, sp->pitch);
                    }
                }
            }
        }

        strncpy(hdr.projectFile, filename, sizeof(hdr.projectFile));
        return true;
    }

    bool Save(const char *filename, std::vector<Sprite*> &sprites) {
        std::ofstream fs(filename, std::ios::trunc | std::ios::binary);
        if(!fs.is_open()) {
            return false;
        }

        fs << PROJECT_FILE_SIGNATURE << CEOL;
        fs << "ProjectName=" << header.projectName << CEOL;
        fs << "Comments=" << replace_string(std::string(header.projectComments), "\n", NEWLINE_PLACEHOLDER) << CEOL;
        fs << "Platform=" << header.projectPlatform << CEOL;
        fs << "CreatedOn=" << header.createdOn << CEOL;
        fs << "AutomaticExportOnSave=" << (header.autoExportSourceCode ? "True" : "False") << CEOL;
        fs << "SourceCodeExportPath=" << header.exportTo << CEOL;
        fs << "AutomaticExportWithComments=" << (header.includeMetadata ? "True" : "False") << CEOL;
        fs << "OnlyData=" << (header.onlyData ? "True" : "False") << CEOL;
        fs << "StartCharIndex=" << header.startCharIndex << CEOL;

        int n = 1;
        for(auto it=sprites.begin(); it != sprites.end(); ++it, n++) {
            Sprite* sp = *it;
            fs << "Sprite" << n << ".ID=" << sp->spriteID << CEOL;
            fs << "Sprite" << n << ".Description=" << replace_string(sp->description, "\n", NEWLINE_PLACEHOLDER) << CEOL;
            fs << "Sprite" << n << ".HeightInPixels=" << sp->heightInPixels << CEOL;
            fs << "Sprite" << n << ".WidthInBytes=" << sp->widthInBytes << CEOL;
            fs << "Sprite" << n << ".ByteAlignment=" << sp->GetByteAlignment() << CEOL;
            fs << "Sprite" << n << ".MulticolorMode=" << (sp->multicolorMode ? "True" : "False") << CEOL;
            fs << "Sprite" << n << ".BackgroundColor=" << WriteRGBColor(sp->backgroundColor) << CEOL;
            fs << "Sprite" << n << ".CharacterColor=" << WriteRGBColor(sp->characterColor) << CEOL;
            fs << "Sprite" << n << ".Multi1Color=" << WriteRGBColor(sp->multi1Color) << CEOL;
            fs << "Sprite" << n << ".Multi2Color=" << WriteRGBColor(sp->multi2Color) << CEOL;
            fs << "Sprite" << n << ".Palette=" << sp->GetPaletteName() << CEOL;
            fs << "Sprite" << n << ".Data=" << Serializator::HexSerializeData(sp->data, sp->widthInBytes, sp->heightInPixels, sp->pitch) << CEOL;
            fs << "Sprite" << n << ".Masked=" << (sp->masked ? "True" : "False") << CEOL;
            fs << "Sprite" << n << ".AnimationAttached=" << (sp->animationAttached ? "True" : "False") << CEOL;
            if(sp->animationAttached) {
                fs << "Sprite" << n << ".AnimationFPS=" << sp->animationFPS << CEOL;
                fs << "Sprite" << n << ".AnimationCount=" << sp->animationFrames.size() << CEOL;
                int a = 0;
                for(auto ai = sp->animationFrames.begin(); ai != sp->animationFrames.end(); ++ai, a++) {
                    auto &an = *ai;
                    fs << "Sprite" << n << ".Frame." << a << "=" << Serializator::HexSerializeData(an.data, sp->widthInBytes, sp->heightInPixels, sp->pitch) << CEOL;
                    if(sp->masked) {
                        fs << "Sprite" << n << ".Mask." << a << "=" << Serializator::HexSerializeData(sp->mask, sp->widthInBytes, sp->heightInPixels, sp->pitch) << CEOL;
                    }
                }
            } else if(sp->masked) {
                fs << "Sprite" << n << ".Mask=" << Serializator::HexSerializeData(sp->mask, sp->widthInBytes, sp->heightInPixels, sp->pitch) << CEOL;
            }
            fs << "Sprite" << n << ".PrerenderSoftwareSprite=" << (sp->prerenderSoftwareSprite ? "True" : "False") << CEOL;
            fs << "Sprite" << n << ".RenderingPrecision=" << sp->GetRenderingPrecision() << CEOL;
            fs << "Sprite" << n << ".ZoomFactor=" << zoomValues[sp->zoomIndex] << CEOL;
        }
        fs.close();
        return true;
    }

    bool ParseFile(const char *filename, keyvalue_list &graph) {
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

    ImU32 ReadRGBColor(const std::string& str) {
        if(str[0] != '#') return 0;
        ImU32 result = 0;
        for(auto it=str.begin()+1; it != str.end(); ++it) {
            size_t d = (*it)-'0'; if(d>=10) d-=7;
            result = result << 4 | d;
        }
        return ABGR_RED(result)<<16|ABGR_GREEN(result)<<8|ABGR_BLUE(result); // RGB -> BGR
    }

    std::string WriteRGBColor(ImU32 color) {
        return vformat("#%02X%02X%02X", ABGR_RED(color), ABGR_GREEN(color), ABGR_BLUE(color));
    }
};
