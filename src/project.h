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
                    //sp->zoomIndex = std::max(0, std::min(7, std::atoi(kv.second.c_str())));
                    auto index = std::atof(kv.second.c_str());
                    auto it = find_if(zoomValues.begin(), zoomValues.end(), [index](auto&&value){ return index == value; });
                    if(it == zoomValues.end()) {
                        sp->zoomIndex = 3;
                    } else {
                        sp->zoomIndex = it - zoomValues.begin();
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

        fs << PROJECT_FILE_SIGNATURE << CR;
        fs << "ProjectName=" << header.projectName << CR;
        fs << "Comments=" << replace_string(std::string(header.projectComments), "\n", NEWLINE_PLACEHOLDER) << CR;
        fs << "Platform=" << header.projectPlatform << CR;
        fs << "CreatedOn=" << header.createdOn << CR;
        fs << "AutomaticExportOnSave=" << (header.autoExportSourceCode ? "True" : "False") << CR;
        fs << "SourceCodeExportPath=" << header.exportTo << CR;
        fs << "AutomaticExportWithComments=" << (header.includeMetadata ? "True" : "False") << CR;
        fs << "OnlyData=" << (header.onlyData ? "True" : "False") << CR;

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
            fs << "Sprite" << n << ".ZoomFactor=" << zoomValues[sp->zoomIndex] << CR;
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
        memset(temp,0,sizeof(temp));
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
                for(size_t t=0, m=128; t<8; ++t, m >>= 1) {
                    data[y*pitch+x+t] = (*p&m) ? 1 : 0;
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
        return ABGR_RED(result)<<16|ABGR_GREEN(result)<<8|ABGR_BLUE(result); // RGB -> BGR
    }

    std::string WriteRGBColor(ImU32 color) {
        return vformat("#%02X%02X%02X", ABGR_RED(color), ABGR_GREEN(color), ABGR_BLUE(color));
    }
};
