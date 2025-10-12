#pragma once

#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include "sprite.h"
#include "project.h"
#include "util.h"

extern const char* PROJECT_FILE_SIGNATURE;

const char *CLIPBOARD_DATA_SIGNATURE = PROJECT_FILE_SIGNATURE;
const char *CEOL = "\n"; // End of line

struct Serializator {

    static std::string Serialize(Sprite *sp) {
        std::stringstream ss;
        ss << CLIPBOARD_DATA_SIGNATURE << CEOL;
        ss << "widthInBytes=" << sp->widthInBytes << CEOL;
        ss << "heightInPixels=" << sp->heightInPixels << CEOL;
        ss << "spriteID=" << sp->spriteID << CEOL;
        ss << "description=" << sp->description << CEOL;
        ss << "multicolorMode=" << sp->multicolorMode << CEOL;
        ss << "byteAlignment=" << (int)sp->byteAligment << CEOL;
        ss << "renderingPrecision=" << (int)sp->renderingPrecision << CEOL;
        ss << "backgroundColor=" << sp->backgroundColor << CEOL;
        ss << "multi1Color=" << sp->multi1Color << CEOL;
        ss << "multi2Color=" << sp->multi2Color << CEOL;
        ss << "characterColor=" << sp->characterColor << CEOL;
        ss << "masked=" << sp->masked << CEOL;
        ss << "data=" << HexSerializeData(sp->data, sp->widthInBytes, sp->heightInPixels,sp->pitch);
        if(sp->masked) {
          ss << "mask=" << HexSerializeData(sp->mask, sp->widthInBytes, sp->heightInPixels,sp->pitch);
        }
        return ss.str();
    }

    static void Deserialize(const char *text, Sprite *sp) {
        // std::cerr << "Clipboard:\n" << text << std::endl;
        std::vector<std::string> tags = split_string(text, CEOL);
        // Ignore data if did not match the signature
        if(tags.front() != CLIPBOARD_DATA_SIGNATURE) return;
        tags.erase(tags.begin()); // remove signature
        for(std::string & st : tags) {
            std::string v = trim(st);
            std::size_t firstEqual = v.find('=');
            if(firstEqual==std::string::npos) continue; // skip invalid line
            std::string key = v.substr(0,firstEqual);
            std::string val = v.substr(firstEqual+1);
            if(key == "widthInBytes") {
                sp->widthInBytes = std::atoi(val.c_str());
            } else if(key == "heightInPixels") {
                sp->heightInPixels = std::atoi(val.c_str());
            } else if(key == "spriteID") {
                strncpy(sp->spriteID, val.c_str(), sizeof(sp->spriteID));
            } else if(key == "description") {
                strncpy(sp->description, val.c_str(), sizeof(sp->description));
            } else if(key == "multicolorMode") {
                sp->multicolorMode = std::atoi(val.c_str())!=0;
            } else if(key == "byteAlignment") {
                sp->byteAligment = (Sprite::ByteAligment)std::atoi(val.c_str());
            } else if(key == "renderingPrecision") {
                sp->renderingPrecision = (Sprite::PrerendingPrecision)std::atoi(val.c_str());
            } else if(key == "backgroundColor") {
                sp->backgroundColor = std::atoi(val.c_str());
            } else if(key == "multi1Color") {
                sp->multi1Color = std::atoi(val.c_str());
            } else if(key == "multi2Color") {
                sp->multi2Color = std::atoi(val.c_str());
            } else if(key == "characterColor") {
                sp->characterColor = std::atoi(val.c_str());
            } else if(key == "masked") {
                sp->masked = std::atoi(val.c_str())!=0;
            } else if(key == "data") {
                HexDeserializeData(val, sp->data, sp->widthInBytes, sp->heightInPixels, sp->pitch);
            } else if(key == "mask") {
                HexDeserializeData(val, sp->mask, sp->widthInBytes, sp->heightInPixels, sp->pitch);
            }
        }
    }

    // NOTE: Width is in bytes
    static std::string HexSerializeData(const char *dt, size_t widthInBytes, size_t heightInPixels, size_t pitch) {
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

    static void HexDeserializeData(const std::string &str, char *data, size_t widthInBytes, size_t heightInPixels, size_t pitch) {
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
};
