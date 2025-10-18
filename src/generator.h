#pragma once
#include "sprite_manager.h"
#include "util.h"

// Width of the byte array line in the source code
#define MAX_BYTE_ARRAY_WIDTH 16

struct Generator {
    SpriteManager *spriteManager;

    Generator(SpriteManager *spriteManager): spriteManager(spriteManager) {}

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
        int n = 1, charIndex = spriteManager->project->header.startCharIndex;
        for(auto sp=spriteManager->sprites.begin();sp!=spriteManager->sprites.end();++sp) {
            auto& sprite = *sp;
            SingleSprite(ss, n++, sprite, charIndex);
            charIndex += sprite->GetCharOffset();
        }
        Footer(ss);

        return ss.str();
    }

    void Header(std::ostream &out) {
        if(spriteManager->exportWithComments || (spriteManager->project->header.autoExportSourceCode && spriteManager->project->header.includeMetadata)) {
            out << vformat("%s ==================== Retro Sprite Workshop Project (%s) ========================", spriteManager->lineCommentSymbol, spriteManager->project->header.projectFile) << std::endl;
            out << vformat("%s Generated On:       %s", spriteManager->lineCommentSymbol, return_current_time_and_date().c_str()) << std::endl;
            out << vformat("%s Project Name:       %s", spriteManager->lineCommentSymbol, spriteManager->project->header.projectName) << std::endl;
            out << vformat("%s Project Comments:   %s", spriteManager->lineCommentSymbol, spriteManager->project->header.projectComments) << std::endl;
            out << vformat("%s Target Platform:    %s", spriteManager->lineCommentSymbol, spriteManager->project->header.projectPlatform) << std::endl;
            out << vformat("%s Project Created On: %s", spriteManager->lineCommentSymbol, spriteManager->project->header.createdOn) << std::endl;
            out << std::endl;
        }
    }

    void Footer(std::ostream &out) {
        if(spriteManager->exportWithComments || (spriteManager->project->header.autoExportSourceCode && spriteManager->project->header.includeMetadata)) {
            out << vformat("%s ==================== Retro Sprite Workshop Project END ========================", spriteManager->lineCommentSymbol) << std::endl;
            out << std::endl;
        }
    }

    void SingleSprite(std::ostream &out, int nr, Sprite *sprite, int charIndex) {
        if(spriteManager->exportWithComments || (spriteManager->project->header.autoExportSourceCode && spriteManager->project->header.includeMetadata)) {
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
            if(sprite->prerenderSoftwareSprite) {
              out << vformat("%s Pre-rendering:   %s",spriteManager->lineCommentSymbol, sprite->GetRenderingPrecision().c_str()) << std::endl;
            } else {
              out << vformat("%s Pre-rendering:   %s",spriteManager->lineCommentSymbol, "No") << std::endl;
            }
            out << vformat("%s Animated:        %s",spriteManager->lineCommentSymbol, sprite->animationAttached ? "Yes" : "No") << std::endl;

            if(sprite->masked) {
              out << vformat("%s Masked:          %s",spriteManager->lineCommentSymbol, sprite->masked ? "Yes" : "No") << std::endl;
            }

            out << std::endl;
        }

        if(!spriteManager->project->header.onlyData) {
            out << Constant(vformat("%s_WIDTH_PX",sprite->spriteID), vformat("%d",sprite->widthInBytes<<3)) << std::endl;
            out << Constant(vformat("%s_HEIGHT_PX",sprite->spriteID), vformat("%d",sprite->heightInPixels)) << std::endl;
            out << Constant(vformat("%s_BIT_PER_PX",sprite->spriteID), vformat("%d",sprite->multicolorMode ? 2 : 1)) << std::endl;
            out << Constant(vformat("%s_INDEX",sprite->spriteID), vformat("%d",nr)) << std::endl;
            out << Constant(vformat("%s_CHAR_INDEX",sprite->spriteID), vformat("%d",charIndex)) << std::endl;
        }
        size_t nframes = 1;
        if(sprite->prerenderSoftwareSprite) {
            switch(sprite->renderingPrecision) {
                case Sprite::PrerendingPrecision::High8Frames: nframes = 8; break;
                case Sprite::PrerendingPrecision::Medium4Frames: nframes = 4; break;
                case Sprite::PrerendingPrecision::Low2Frames: nframes = 2; break;
            }
        }
        if(!spriteManager->project->header.onlyData) {
            if(sprite->animationAttached) {
                out << Constant(vformat("%s_ANIM_SIZE",sprite->spriteID), vformat("%d",sprite->animationFrames.size()*nframes)) << std::endl;
                out << Constant(vformat("%s_ANIM_NUM_FRAMES",sprite->spriteID), vformat("%d",sprite->animationFrames.size())) << std::endl;
                out << Constant(vformat("%s_ANIM_FPS",sprite->spriteID), vformat("%d",sprite->animationFPS)) << std::endl;
            }
            out << Constant(vformat("%s_NUM_IMAGES",sprite->spriteID), vformat("%d",nframes)) << std::endl;
            out << std::endl;
        }

        MakeFrames(out, sprite);
    }

    void ShiftSpriteFrame(std::ostream &out, const char *data_type, int frame, Sprite *sp, char *data) {
        // copy sprite data into temporary shift buffer
        bool masked = (strcmp(data_type, "mask") == 0);
        char buffer[4096+64*8]; size_t bpitch = 64+8;
        memset((void*)buffer, (masked ? 0x01 : 0x00), sizeof(buffer));
        size_t widthInPixels = sp->widthInBytes<<3;
        for(size_t y=0;y<sp->heightInPixels;++y) {
            for(size_t x=0;x<widthInPixels;++x) {
                buffer[y*bpitch+x] = data[y*sp->pitch+x];
            }
        }
        switch(sp->renderingPrecision) {
            case Sprite::PrerendingPrecision::High8Frames:
            for(size_t sh=0;sh<8;++sh) {
                SingleFrame(out, sp, data_type, frame, sh, buffer, widthInPixels+8, sp->heightInPixels, bpitch);
                ShiftBuffer(sp, buffer, bpitch);
            }
            break;
            case Sprite::PrerendingPrecision::Medium4Frames:
            for(size_t sh=0;sh<4;++sh) {
                SingleFrame(out, sp, data_type, frame, sh, buffer, widthInPixels+8, sp->heightInPixels, bpitch);
                ShiftBuffer(sp, buffer, bpitch);
                ShiftBuffer(sp, buffer, bpitch);
            }
            break;
            case Sprite::PrerendingPrecision::Low2Frames:
            for(size_t sh=0;sh<2;++sh) {
                SingleFrame(out, sp,data_type,  frame, sh, buffer, widthInPixels+8, sp->heightInPixels, bpitch);
                ShiftBuffer(sp, buffer, bpitch);
                ShiftBuffer(sp, buffer, bpitch);
                ShiftBuffer(sp, buffer, bpitch);
                ShiftBuffer(sp, buffer, bpitch);
            }
            break;
        }
    }

    void MakeFrames(std::ostream &out, Sprite *sp) {
        if(sp->prerenderSoftwareSprite) {
            if(sp->animationAttached) {
                for(size_t fr=0;fr<sp->animationFrames.size();++fr) {
                    ShiftSpriteFrame(out, "frame", fr, sp, sp->animationFrames[fr].data);
                    if(sp->masked) {
                      ShiftSpriteFrame(out, "mask", fr, sp, sp->animationFrames[fr].mask);
                    }
                }
            } else {
                ShiftSpriteFrame(out, "image", -1, sp, sp->data);
                if(sp->masked) {
                  ShiftSpriteFrame(out, "mask", -1, sp, sp->mask);
                }
            }
        } else {
            if(sp->animationAttached) {
                for(size_t fr=0;fr<sp->animationFrames.size();++fr) {
                    SingleFrame(out, sp, "frame", fr, -1, sp->animationFrames[fr].data, sp->widthInBytes<<3, sp->heightInPixels, sp->pitch);
                    if(sp->masked) {
                      SingleFrame(out, sp, "mask", fr, -1, sp->animationFrames[fr].mask, sp->widthInBytes<<3, sp->heightInPixels, sp->pitch);    
                    }
                }
            } else {
                SingleFrame(out, sp, "image", -1, -1, sp->data, sp->widthInBytes<<3, sp->heightInPixels, sp->pitch);
                if(sp->masked) {
                    SingleFrame(out, sp, "mask", -1, -1, sp->mask, sp->widthInBytes<<3, sp->heightInPixels, sp->pitch); 
                }
            }
        }
    }

    // Naming convention
    // shift = 0..n (-1 no shift)
    // data_type = image | mask
    // <spriteID>_<data_type>
    // <spriteID>_<data_type>_shift<n>
    // <spriteID>_<data_type>_frame<n>
    // <spriteID>_<data_type>_frame<n>_shift<n>
    void SingleFrame(std::ostream &out, Sprite *sprite, const char *data_type, int frame, int shift, const char *data, size_t widthInPixels, size_t heightInPixels, size_t pitch) {
        if(!spriteManager->project->header.onlyData) {
            if(sprite->animationAttached) {
                if(shift!=-1) {
                    out << Label(vformat("%s_%s%d_shift%d", sprite->spriteID, data_type, frame, shift)) << std::endl;
                } else {
                    out << Label(vformat("%s_%s%d", sprite->spriteID, data_type, frame)) << std::endl;
                }
            } else {
                if(shift!=-1) {
                    out << Label(vformat("%s_%s_shift%d", sprite->spriteID, data_type, shift)) << std::endl;
                } else {
                    out << Label(vformat("%s_%s", sprite->spriteID, data_type)) << std::endl;
                }
            }
        }
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

        if(!spriteManager->project->header.onlyData) {
            out << std::endl;
        }
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
                char c1 = p[widthInPixels - 1], c2= p[widthInPixels - 2];
                for (int k = widthInPixels - 2; k >= 2; k-=2) {
                    p[k+0] = p[k-2];
                    p[k+1] = p[k-1];
                }
                p[0] = c2;
                p[1] = c1;
            } else {
                char c = *(p + widthInPixels - 1);
                for (int k = widthInPixels - 1; k >= 0; --k) {
                    *(p + k) = *(p + k - 1);
                }
                *p = c;
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
