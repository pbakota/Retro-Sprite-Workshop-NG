#pragma once
#include "sprite_manager.h"
#include "util.h"

// Width of the byte array line in the source code
#define MAX_BYTE_ARRAY_WIDTH 16

struct Generator {
    SpriteManager *spriteManager;
    Project *project;

    Generator(SpriteManager *spriteManager, Project *project): spriteManager(spriteManager), project(project) {}

    bool GenerateToFile(const char *filename) {
        std::ofstream file(filename, std::ios::binary|std::ios::trunc);
        if(!file.is_open()) return false;

        std::string content = GenerateToString();
        file << content;

        file.close();
        return true;
    }

    std::string GenerateToString() {
        std::stringstream ss;

        Header(ss);
        int n = 1;
        for(auto sp=spriteManager->sprites.begin();sp!=spriteManager->sprites.end();++sp) {
            SingleSprite(ss, n++, *sp);
        }
        Footer(ss);

        return ss.str();
    }

    void Header(std::ostream &out) {
        out << vformat("%s ==================== Retro Sprite Workshop Project (%s) ========================", spriteManager->lineCommentSymbol, project->projectFile) << std::endl;
        out << vformat("%s Generated On:       %s", spriteManager->lineCommentSymbol, return_current_time_and_date().c_str()) << std::endl;
        out << vformat("%s Project Name:       %s", spriteManager->lineCommentSymbol, project->projectName) << std::endl;
        out << vformat("%s Project Comments:   %s", spriteManager->lineCommentSymbol, project->projectComments) << std::endl;
        out << vformat("%s Target Platform:    %s", spriteManager->lineCommentSymbol, project->projectPlatform) << std::endl;
        out << vformat("%s Project Created On: %s", spriteManager->lineCommentSymbol, project->createdOn) << std::endl;
        out << std::endl;
    }

    void Footer(std::ostream &out) {
        out << vformat("%s ==================== Retro Sprite Workshop Project END ========================", spriteManager->lineCommentSymbol) << std::endl;
        out << std::endl;
    }

    void SingleSprite(std::ostream &out, int nr, Sprite *sprite) {

        out << vformat("%s ==================== Sprite %d ========================", spriteManager->lineCommentSymbol, nr) << std::endl;
        out << vformat("%s Sprite ID:       %s", spriteManager->lineCommentSymbol, sprite->spriteID) << std::endl;


        std::vector v = split_string(sprite->description, "\n");
        bool firstLine = true;
        for(auto it=v.begin(); it!=v.end(); ++it) {
            if(firstLine) {
                out << vformat("%s Sprite Comments: %s",spriteManager->lineCommentSymbol, (*it).c_str()) << std::endl;
                firstLine = false;
            } else {
                out << vformat("%s                  %s",spriteManager->lineCommentSymbol, (*it).c_str()) << std::endl;
            }
        }

        out << vformat("%s Dimensions:      %d x %d pixels", spriteManager->lineCommentSymbol, sprite->widthInBytes<<3, sprite->heightInPixels) << std::endl;
        out << vformat("%s Color Mode:      %s",spriteManager->lineCommentSymbol, ColorMode(sprite)) << std::endl;
        out << vformat("%s Byte Order:      %s",spriteManager->lineCommentSymbol, sprite->GetByteAlignment().c_str()) << std::endl;
        out << vformat("%s Pre-rendering:   %s",spriteManager->lineCommentSymbol, sprite->GetRenderingPrecision().c_str()) << std::endl;

        out << std::endl;

        out << Constant(vformat("%s_WIDTH_PX",sprite->spriteID), vformat("%d",sprite->widthInBytes<<3)) << std::endl;
        out << Constant(vformat("%s_HEIGHT_PX",sprite->spriteID), vformat("%d",sprite->heightInPixels)) << std::endl;
        out << Constant(vformat("%s_BIT_PER_PX",sprite->spriteID), vformat("%d",sprite->multicolorMode ? 2 : 1)) << std::endl;
        out << Constant(vformat("%s_INDEX",sprite->spriteID), vformat("%d",sprite->charIndex)) << std::endl;
        out << Constant(vformat("%s_CHAR_INDEX",sprite->spriteID), vformat("%d",sprite->charOffset<<3)) << std::endl;
        size_t nframes = 1;
        if(sprite->prerenderSoftwareSprite) {
            switch(sprite->renderingPrecision) {
                case Sprite::PrerendingPrecision::High8Frames: nframes = 8; break;
                case Sprite::PrerendingPrecision::Medium4Frames: nframes = 4; break;
                case Sprite::PrerendingPrecision::Low2Frames: nframes = 2; break;
            }
        }
        out << Constant(vformat("%s_NUM_FRAMES",sprite->spriteID), vformat("%d",nframes)) << std::endl;

        out << std::endl;

        MakeFrames(out, sprite);
    }

    void MakeFrames(std::ostream &out, Sprite *sp) {
        if(sp->prerenderSoftwareSprite) {
            // copy sprite data into temporary shift buffer
            char buffer[4096+64*8]; size_t bpitch = 64+8;
            memset((void*)buffer, 0, sizeof(buffer));
            size_t widthInPixels = sp->widthInBytes<<3;
            for(size_t y=0;y<sp->heightInPixels;++y) {
                for(size_t x=0;x<widthInPixels;++x) {
                    buffer[y*bpitch+x] = sp->data[y*sp->pitch+x];
                }
            }
            switch(sp->renderingPrecision) {
                case Sprite::PrerendingPrecision::High8Frames:
                for(size_t n=0;n<8;++n) {
                    SingleFrame(out, sp, n, buffer, widthInPixels+8, sp->heightInPixels, bpitch);
                    ShiftBuffer(sp, buffer, bpitch);
                }
                break;
                case Sprite::PrerendingPrecision::Medium4Frames:
                for(size_t n=0;n<4;++n) {
                    SingleFrame(out, sp, n, buffer, widthInPixels+8, sp->heightInPixels, bpitch);
                    ShiftBuffer(sp, buffer, bpitch);
                    ShiftBuffer(sp, buffer, bpitch);
                }
                break;
                case Sprite::PrerendingPrecision::Low2Frames:
                for(size_t n=0;n<2;++n) {
                    SingleFrame(out, sp, n, buffer, widthInPixels+8, sp->heightInPixels, bpitch);
                    ShiftBuffer(sp, buffer, bpitch);
                    ShiftBuffer(sp, buffer, bpitch);
                    ShiftBuffer(sp, buffer, bpitch);
                    ShiftBuffer(sp, buffer, bpitch);
                }
                break;
            }
        } else {
            SingleFrame(out, sp, 0, sp->data, sp->widthInBytes<<3, sp->heightInPixels, sp->pitch);
        }
    }

    void SingleFrame(std::ostream &out, Sprite *sprite, int nr, const char *data, size_t widthInPixels, size_t heightInPixels, size_t pitch) {
        out << Label(vformat("%s_frame%d", sprite->spriteID, nr)) << std::endl;
        std::vector<std::string> nibbles;
        switch(sprite->byteAligment) {
            case Sprite::ByteAligment::Horizontal_C64_Sprite:
                nibbles = HexSerializeData_Horizontal_C64_Sprite(data, widthInPixels, heightInPixels, pitch);
            break;
            case Sprite::ByteAligment::Vertical_Software_Sprite:
                nibbles = HexSerializeData_Vertical_Software_Sprite(data, widthInPixels, heightInPixels, pitch);
            break;
            case Sprite::ByteAligment::Mixed_Character_Based:
                nibbles = HexSerializeData_Mixed_Character_Based(data, widthInPixels, heightInPixels, pitch);
            break;
        }

        out << vformat("                            %s ", spriteManager->byteArrayType);

        size_t n = 0;
        for(auto &s : nibbles) {
            out << s;
            if((++n%MAX_BYTE_ARRAY_WIDTH)!=0 && n<nibbles.size()) {
                out << ",";
            } else  {
                out << std::endl;
                if(n<nibbles.size()) {
                    out << vformat("                            %s ", spriteManager->byteArrayType);
                }
            }
        }

        out << std::endl;
    }

    std::vector<std::string> HexSerializeData_Horizontal_C64_Sprite(const char *data, size_t widthInPixels, size_t heightInPixels, size_t pitch) {
        std::vector<std::string> ss;
        for(size_t y=0; y<heightInPixels; ++y) {
            for(size_t x=0; x<widthInPixels; x+=8) {
                size_t b = 0;
                for(size_t p=0; p<8;++p) {
                    b = (b << 1) | data[y*pitch+x+p];
                }
                ss.emplace_back(vformat("$%02x", b&0xff));
            }
        }
        return ss;
    }

    std::vector<std::string> HexSerializeData_Vertical_Software_Sprite(const char *data, size_t widthInPixels, size_t heightInPixels, size_t pitch) {
        std::vector<std::string> ss;
        for(size_t x=0; x<widthInPixels; x+=8) {
            for(size_t y=0; y<heightInPixels; ++y) {
                size_t b = 0;
                for(size_t p=0; p<8;++p) {
                    b = (b << 1) | data[y*pitch+x+p];
                }
                ss.emplace_back(vformat("$%02x", b&0xff));
            }
        }
        return ss;
    }

    std::vector<std::string> HexSerializeData_Mixed_Character_Based(const char *data, size_t widthInPixels, size_t heightInPixels, size_t pitch) {
        std::vector<std::string> ss;
        for(size_t y=0; y<heightInPixels; y+=8) {
            for(size_t x=0; x<widthInPixels; x+=8) {
                for(size_t l=0;l<8;++l) {
                    size_t b = 0;
                    for(size_t p=0; p<8;++p) {
                        b = (b << 1) | data[(y+l)*pitch+x+p];
                    }
                    ss.emplace_back(vformat("$%02x", b&0xff));
                }
            }
        }
        return ss;
    }

    void ShiftBuffer(Sprite* sp, char *buffer, size_t pitch) {
        size_t widthInPixels = (sp->widthInBytes+1)<<3;
        for (size_t j = 0; j < sp->heightInPixels; ++j)
		{
            char *p = &buffer[j * pitch];
            if(sp->multicolorMode) {
                for (int k = widthInPixels - 2; k >= 2; k-=2) {
                    p[k+0] = p[k-2];
                    p[k+1] = p[k-1];
                }
                p[0] = 0;
                p[1] = 0;
            } else {
                for (int k = widthInPixels - 1; k >= 0; --k) {
                    *(p + k) = *(p + k - 1);
                }
                *p = 0;
            }
		}
    }

    inline std::string Constant(const std::string &name, const std::string &value) {
        return replace_string(replace_string(spriteManager->constantDeclaration, "{{NAME}}", name), "{{VALUE}}", value);
    }

    inline std::string Label(const std::string &label) {
        return replace_string(spriteManager->labelDeclaration, "{{LABEL}}", label);
    }

    inline const char *ColorMode(Sprite *sp) {
        return (sp->multicolorMode ? "Multicolor (4 colors, 2 bits per pixel)" : "2 colors");
    }
};
