#pragma once
#include "sprite_manager.h"
#include "util.h"

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
        out << Constant(vformat("%s_NUM_FRAMES",sprite->spriteID), vformat("%d",0)) << std::endl;

        out << std::endl;

        SingleFrame(out, 0, sprite);
    }

    void SingleFrame(std::ostream &out, int nr, Sprite *sprite) {
        out << Label(vformat("%s_frame%d", sprite->spriteID, nr)) << std::endl;
        out << vformat("                            %s ", spriteManager->byteArrayType);

        out << HexSerializeData_Horizontal_C64_Sprite(sprite) << std::endl;

        out << std::endl;
    }

    std::string HexSerializeData_Horizontal_C64_Sprite(Sprite *sprite) {
        std::stringstream ss;
        size_t widthInPixels = sprite->widthInBytes<<3;
        for(size_t y=0; y<sprite->heightInPixels; ++y) {
            for(size_t x=0; x<widthInPixels; x+=8) {
                size_t b = 0;
                for(size_t p=0; p<8;++p) {
                    b = (b << 1) | sprite->data[y*sprite->pitch+x+p];
                }
                ss << vformat("$%02x, ", b);
            }
        }
        return ss.str();
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
