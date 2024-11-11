#pragma once
#include <vector>
#include <algorithm>
#include <memory>
#include <cstdlib>
#include "sprite.h"
#include "project.h"
#include "sprite_manager.h"
#include "statusbar.h"

struct SpriteManager {
    Project *project;
    StatusBar *statusbar;

    Sprite* currentSprite = nullptr;
    SDL_Texture *captureScreenshot = nullptr;
    SDL_Surface *captureSurface = nullptr;
    ImVec2 captureScreenshotSize = ImVec2(-FLT_MIN, -FLT_MIN);

    const size_t MAX_MRU_ENTRIES = 5;

    std::vector<Sprite*> sprites;
    std::string projectFile = ""; //"project.spr";
    int spriteListType = 0;
    std::vector<std::string> MRU;
    std::vector<std::string> captureMRU;
    const char *configPath;
    std::string configFile;
    bool exporWithComments = false;
    bool shiftRollingAround = true;

    char lineCommentSymbol[8] = ";";
    char byteArrayType[8] = "byt";
    char constantDeclaration[128] = "{{NAME}} = {{VALUE}}";
    char labelDeclaration[128] = "{{LABEL}}";

    SpriteManager(Project *project, StatusBar* statusbar) : project(project), statusbar(statusbar) {
        captureMRU.emplace_back("/home/sorel/work/p4tools/src/bruce_lee_sprites2.spr");
        captureMRU.emplace_back("/home/sorel/work/p4tools/src/project3.spr");
        captureMRU.emplace_back("/home/sorel/work/p4tools/src/charset.spr");
        captureMRU.emplace_back("/home/sorel/work/p4tools/src/examples/charset.spr");
        captureMRU.emplace_back("/home/sorel/work/p4tools/src/figure1.spr");
    }

    ~SpriteManager() {
        ClearSpriteList();
        SDL_free((void*)configPath);
        if(captureScreenshot) {
            SDL_DestroyTexture(captureScreenshot);
        }
        if(captureSurface) {
            SDL_FreeSurface(captureSurface);
        }
    }

    void ClearSpriteList() {
        for (auto it = sprites.begin(); it != sprites.end(); ++it) {
            delete *it;
        }
        sprites.clear();
    }

    Sprite *GetSprite(int id) {
        auto current = find_if(sprites.begin(), sprites.end(), [id](auto&&sp) { return (sp->ID == id); });
        assert(current != sprites.end());
        return *current;
    }

    int MoveUp(int id) {
        auto current = find_if(sprites.begin(), sprites.end(), [id](auto&&sp) { return (sp->ID == id); });
        if(current == sprites.begin()) return (*current)->ID;
        auto ci = current - sprites.begin();
        auto prev = sprites.begin() + (ci-1);
        std::swap(*current, *prev);
        return (*prev)->ID;
    }

    int MoveDown(int id) {
        auto current = find_if(sprites.begin(), sprites.end(), [id](auto&&sp) { return (sp->ID == id); });
        if(current == sprites.end()-1) return (*current)->ID;
        auto ci = current - sprites.begin();
        auto next = sprites.begin() + (ci+1);
        std::swap(*current, *next);
        return (*next)->ID;
    }

    void ClearSprite(int id, size_t color) {
        Sprite *sp = GetSprite(id);
        if(!sp->multicolorMode&&(color==1||color==2)) return;
        size_t widthInPixels = sp->widthInBytes<<3;
        for(size_t y=0;y<sp->heightInPixels;++y) {
            if(sp->multicolorMode) {
                for(size_t x=0; x<widthInPixels;x+=2) {
                    sp->data[y*sp->pitch+x+0] = color>>1;
                    sp->data[y*sp->pitch+x+1] = color&1;
                }
            } else {
                for(size_t x=0;x<widthInPixels;++x) {
                    sp->data[y*sp->pitch+x] = color&1;
                }
            }
        }
        sp->Invalidate();
    }

    void FlipImage_Horizontal(int id) {
        Sprite *sp = GetSprite(id);
        size_t widthInPixels = sp->widthInBytes<<3;
        for (size_t y = 0; y < sp->heightInPixels; ++y) {
            if(sp->multicolorMode) {
                for (size_t x = 0; x < widthInPixels / 2; x+=2) {
                    char p1 = sp->data[y * sp->pitch + x+0], p2 = sp->data[y * sp->pitch + x+1];
                    sp->data[y * sp->pitch + x+0] = sp->data[y * sp->pitch + ((widthInPixels - 1) - x-1)];
                    sp->data[y * sp->pitch + x+1] = sp->data[y * sp->pitch + ((widthInPixels - 1) - x-0)];
                    sp->data[y * sp->pitch + ((widthInPixels - 1) - x)-1] = p1;
                    sp->data[y * sp->pitch + ((widthInPixels - 1) - x)-0] = p2;
                }
            } else {
                for (size_t x = 0; x < widthInPixels / 2; ++x) {
                    char val = sp->data[y * sp->pitch + x];
                    sp->data[y * sp->pitch + x] = sp->data[y * sp->pitch + ((widthInPixels - 1) - x)];
                    sp->data[y * sp->pitch + ((widthInPixels - 1) - x)] = val;
                }
            }
        }
        sp->Invalidate();
    }

    void FlipImage_Vertical(int id) {
        Sprite *sp = GetSprite(id);
        char tmp[sp->pitch];
        for (size_t y = 0; y < sp->heightInPixels / 2; ++y) {
            memcpy((void *)tmp, (void *)&sp->data[y * sp->pitch], sp->pitch);
            memcpy((void *)&sp->data[y * sp->pitch], (void *)&sp->data[((sp->heightInPixels - 1) - y) * sp->pitch], sp->pitch);
            memcpy((void *)&sp->data[((sp->heightInPixels - 1) - y) * sp->pitch], (void *)tmp, sp->pitch);
        }
        sp->Invalidate();
    }

    void ShiftImage_Up(int id, bool rotate) {
        Sprite *sp = GetSprite(id);
        if(sp->heightInPixels == 1) return;
        char tmp[sp->pitch];
        size_t widthInPixels = sp->widthInBytes<<3;
		memcpy((void *)tmp, (void *)sp->data, widthInPixels);
		for (size_t j = 1; j < sp->heightInPixels; ++j) {
			memcpy((void *)&sp->data[(j - 1) * sp->pitch], (void *)&sp->data[j * sp->pitch], widthInPixels);
		}
        if(rotate) {
		    memcpy((void *)&sp->data[(sp->heightInPixels - 1) * sp->pitch], (void *)&tmp, widthInPixels);
        } else {
            memset((void *)&sp->data[(sp->heightInPixels - 1) * sp->pitch], 0, widthInPixels);
        }
        sp->Invalidate();
    }

    void ShiftImage_Down(int id, bool rotate) {
        Sprite *sp = GetSprite(id);
        if(sp->heightInPixels == 1) return;
        char tmp[sp->pitch];
        size_t widthInPixels = sp->widthInBytes<<3;
		memcpy((void *)tmp, (void *)&sp->data[(sp->heightInPixels - 1) * sp->pitch], widthInPixels);
		for (int j = sp->heightInPixels - 2; j >= 0; --j) {
			memcpy((void *)&sp->data[(j + 1) * sp->pitch], (void *)&sp->data[j * sp->pitch], widthInPixels);
		}
        if(rotate) {
		    memcpy((void *)sp->data, (void *)&tmp, widthInPixels);
        } else {
            memset((void *)sp->data, 0, widthInPixels);
        }
        sp->Invalidate();
    }

    void ShiftImage_Left(int id, bool rotate) {
        Sprite *sp = GetSprite(id);
        size_t widthInPixels = sp->widthInBytes<<3;
        for (size_t j = 0; j < sp->heightInPixels; ++j) {
            if(sp->multicolorMode) {
                char *p = &sp->data[j * sp->pitch], c1 = p[0], c2 = p[1];
                for (size_t k = 2; k < widthInPixels; k+=2) {
                    p[k-2] = p[k+0];
                    p[k-1] = p[k+1];
                }
                p[widthInPixels - 2] = rotate ? c1 : 0;
                p[widthInPixels - 1] = rotate ? c2 : 0;
            } else {
                char *p = &sp->data[j * sp->pitch], c = *p;
                for (size_t k = 1; k < widthInPixels; ++k) {
                    *(p + k - 1) = *(p + k);
                }
                *(p + widthInPixels - 1) = rotate ? c : 0;
            }
		}
        sp->Invalidate();
    }

    void ShiftImage_Right(int id, bool rotate) {
        Sprite *sp = GetSprite(id);
        size_t widthInPixels = sp->widthInBytes<<3;
        for (size_t j = 0; j < sp->heightInPixels; ++j)
		{
            if(sp->multicolorMode) {
                char *p = &sp->data[j * sp->pitch], c1 = p[widthInPixels - 1], c2= p[widthInPixels - 2];
                for (int k = sp->heightInPixels - 2; k >= 2; k-=2) {
                    p[k+0] = p[k-2];
                    p[k+1] = p[k-1];
                }
                p[0] = rotate ? c2 : 0;
                p[1] = rotate ? c1 : 0;
            } else {
                char *p = &sp->data[j * sp->pitch], c = *(p + widthInPixels - 1);
                for (int k = sp->heightInPixels - 1; k >= 0; --k) {
                    *(p + k) = *(p + k - 1);
                }
                *p = rotate ? c : 0;
            }
		}
        sp->Invalidate();
    }

    void RotateImage_Clockwise(int id) {
        Sprite *sp = GetSprite(id);
        if(sp->widthInBytes<<3!=sp->heightInPixels) return;
        char tmp[4096]; memcpy(tmp, sp->data, sizeof(sp->data));
        if(sp->multicolorMode) {
            FlipImage_Horizontal(id);
            FlipImage_Vertical(id);
        } else {
            for(size_t y=0;y<sp->heightInPixels;++y) {
                for(size_t x=0;x<sp->heightInPixels;++x) {
                    sp->data[y*sp->pitch+(sp->heightInPixels-x)-1] = tmp[x*sp->pitch+y];
                }
            }
        }
        sp->Invalidate();
    }

    void RotateImage_CounterClockwise(int id) {
        Sprite *sp = GetSprite(id);
        if(sp->widthInBytes<<3!=sp->heightInPixels) return;
        char tmp[4096]; memcpy(tmp, sp->data, sizeof(sp->data));
        if(sp->multicolorMode) {
            FlipImage_Vertical(id);
            FlipImage_Horizontal(id);
        } else {
            for(size_t y=0;y<sp->heightInPixels;++y) {
                for(size_t x=0;x<sp->heightInPixels;++x) {
                    sp->data[y*sp->pitch+x] = tmp[x*sp->pitch+(sp->heightInPixels-y)-1];
                }
            }
        }
        sp->Invalidate();
    }

    void InsertRow(int id, size_t row) {
        Sprite *sp = GetSprite(id);
        size_t widthInPixels = sp->widthInBytes<<3;
        for(int y=sp->heightInPixels-2;y>=(int)row;--y) {
            for(size_t x=0;x<widthInPixels;++x) {
                sp->data[(y+1)*sp->pitch+x] = sp->data[y*sp->pitch+x];
            }
        }
        memset(&sp->data[row*sp->pitch], 0, widthInPixels);
        sp->Invalidate();
    }

    void RemoveRow(int id, size_t row) {
        Sprite *sp = GetSprite(id);
        size_t widthInPixels = sp->widthInBytes<<3;
        for(size_t y=row;y<sp->heightInPixels-1;++y) {
            for(size_t x=0;x<widthInPixels;++x) {
                sp->data[y*sp->pitch+x] = sp->data[(y+1)*sp->pitch+x];
            }
        }
        memset(&sp->data[(sp->heightInPixels-1)*sp->pitch], 0, widthInPixels);
        sp->Invalidate();
    }

    void InsertColumn(int id, size_t col) {
        Sprite *sp = GetSprite(id);
        size_t widthInPixels = sp->widthInBytes<<3;
        for(size_t y=0;y<sp->heightInPixels;++y) {
            if(sp->multicolorMode) {
                for(int x=widthInPixels-4;x>=(int)col*2;x-=2) {
                    sp->data[y*sp->pitch+x+2] = sp->data[y*sp->pitch+x+0];
                    sp->data[y*sp->pitch+x+3] = sp->data[y*sp->pitch+x+1];
                }
                sp->data[y*sp->pitch+col+0] = 0;
                sp->data[y*sp->pitch+col+1] = 0;
            } else {
                for(int x=widthInPixels-2;x>=(int)col;--x) {
                    sp->data[y*sp->pitch+x+1] = sp->data[y*sp->pitch+x];
                }
                sp->data[y*sp->pitch+col] = 0;
            }
        }
        sp->Invalidate();
    }

    void RemoveColumn(int id, size_t col) {
        Sprite *sp = GetSprite(id);
        size_t widthInPixels = sp->widthInBytes<<3;
        for(size_t y=0;y<sp->heightInPixels;++y) {
            for(size_t x=col;x<widthInPixels-1;++x) {
                sp->data[y*sp->pitch+x] = sp->data[y*sp->pitch+x+1];
            }
            sp->data[y*sp->pitch+widthInPixels-1] = 0;
        }
        sp->Invalidate();
    }

    void RearrangeColors(int id, size_t selected[]) {
        Sprite *sp = GetSprite(id);
        char tmp[4096]; memcpy(tmp, sp->data, sizeof(tmp));
        for(size_t n=0;n<4;++n) {
            if(selected[n] != n) {
                SwapColorPixels(sp, tmp, n, selected[n]);
            }
        }
        sp->Invalidate();
    }

    void SwapColorPixels(Sprite *sp, char *original, char from, char to) {
        size_t widthInPixels = sp->widthInBytes<<3;
        for(size_t y=0;y<sp->heightInPixels;++y) {
            if(sp->multicolorMode) {
                for(size_t x=0;x<widthInPixels;x+=2) {
                    char b = (original[y*sp->pitch+x+0] << 1) | original[y*sp->pitch+x+1];
                    if(b == from) {
                        sp->data[y*sp->pitch+x+0] = (to>>1);
                        sp->data[y*sp->pitch+x+1] = (to&1);
                    }
                }
            } else {
                for(size_t x=0;x<widthInPixels;++x) {
                    if(original[y*sp->pitch+x] == (from&1)) {
                        sp->data[y*sp->pitch+x] = (to&1);
                    }
                }
            }
        }
    }

    void NewProject() {
        DetachSprite();
        ClearSpriteList();
        // Always append one (default) sprite
        NewSprite();
        project->NewProject();
    }

    int NewSprite() {
        Sprite* sprite = new Sprite(1,8, "sprite");
        sprite->byteAligment = Sprite::ByteAligment::Mixed_Character_Based;
        sprite->multicolorMode = false;
        sprite->prerenderSoftwareSprite = false;
        sprite->renderingPrecision = Sprite::PrerendingPrecision::Medium4Frames;
        // TODO: Copy last used colors into the new sprite

        AppendSprite(sprite);
        return sprite->ID;
    }

    // Append new sprite at the end of the list
    void AppendSprite(Sprite *sprite) {
        sprites.push_back(sprite);
    }

    int CloneSprite(int id) {
        auto current = std::find_if(sprites.begin(), sprites.end(), [id](auto&&sp) { return (sp->ID == id); });
        // if not found, exit
        if(current == sprites.end()) return -1;
        Sprite *fromSp = *current;
        return CloneSprite(fromSp);
    }

    int CloneSprite(const Sprite* fromSp) {
        int newId = NewSprite();
        auto it = std::find_if(sprites.begin(), sprites.end(), [newId](auto&&sp) { return (sp->ID == newId); });
        Sprite *toSp = *it;

        toSp->heightInPixels = fromSp->heightInPixels;
        toSp->widthInBytes = fromSp->widthInBytes;
        //char spriteID[128] = {0};
        toSp->byteAligment = fromSp->byteAligment;
        toSp->multicolorMode = fromSp->multicolorMode;
        toSp->prerenderSoftwareSprite = fromSp->prerenderSoftwareSprite;
        toSp->renderingPrecision = fromSp->renderingPrecision;
        toSp->backgroundColor = fromSp->backgroundColor;
        toSp->multi1Color     = fromSp->multi1Color;
        toSp->multi2Color     = fromSp->multi2Color;
        toSp->characterColor  = fromSp->characterColor;
        toSp->palette = fromSp->palette;
        toSp->zoomIndex = fromSp->zoomIndex;
        memcpy(toSp->description, fromSp->description, sizeof(toSp->description));
        memcpy(toSp->data, fromSp->data, sizeof(toSp->data));
        toSp->Invalidate();
        return toSp->ID;
    }

    // Remove sprite at index
    void RemoveSprite(int id) {
        sprites.erase(std::find_if(sprites.begin(), sprites.end(), [id](auto&&sp) { bool c = (sp->ID == id); if(c) { delete sp; } return c; }));
        if(sprites.size() == 0) {
            // Must have at least one sprite
            NewSprite();
        }
    }

    // Insert sprite after index
    void InsertSprite(const std::size_t index, Sprite *sprite) {
        sprites.insert(sprites.begin() + index, sprite);
    }

    int NextSpriteID(int id) {
        auto current = std::find_if(sprites.begin(), sprites.end(), [id](auto&&sp) { return (sp->ID == id); });
        // if not found, exit
        if(current == sprites.end()) return -1;

        // try next item id
        if((current+1) != sprites.end()) {
            return (*(current+1))->ID;
        }
        // ... or if not avaialble, then try previous
        if((current-1) == sprites.begin()) return -1;
        return (*(current-1))->ID;
    }

    void AttachSprite(Sprite* sprite) {
        currentSprite = sprite;
        statusbar->is_zoom_visible = true;
        statusbar->zoomIndex = sprite->zoomIndex;
    }

    void DetachSprite() {
        currentSprite = nullptr;
        statusbar->is_zoom_visible = false;
    }

    bool LoadProject(const std::string& filename) {
        std::vector<Sprite*> temp;
        bool result = project->Load(filename, temp);
        if(result) {
            DetachSprite();
            ClearSpriteList();
            sprites = temp;
            projectFile = filename;
            AddToMRU(filename);
        }

        return result;
    }

    void AddToMRU(const std::string& filename) {
        // TODO: Implement a better MRU. e.g. If the item is already on the list, then it should be moved to the bottom (top?) of the list.
        if(std::find_if(MRU.begin(), MRU.end(), [filename](auto&&e) { return e == filename; }) == MRU.end()) {
            if(MRU.size() >= MAX_MRU_ENTRIES) {
                MRU.erase(MRU.begin()); // remove from top
            }
            MRU.emplace_back(filename);
        }
    }

    bool SaveProject() {
        return project->Save(projectFile, sprites);
    }

    bool SaveProjectAs(const std::string& filename) {
        if(project->Save(filename, sprites)) {
            projectFile = filename;
            AddToMRU(filename);
            return true;
        }
        return false;
    }

    void SaveConfig() {
        std::ofstream cfg(configFile, std::ios::binary|std::ios::trunc);
        if(!cfg.is_open()) return;

        cfg << "ExporWithComments=" << (exporWithComments ? "True":"False") << std::endl;
        cfg << "ShiftRollingAround=" << (shiftRollingAround ? "True":"False") << std::endl;

        cfg << "LineCommentSymbol=" << lineCommentSymbol << std::endl;
        cfg << "ByteArrayType=" << byteArrayType << std::endl;
        cfg << "ConstantDeclaration=" << constantDeclaration << std::endl;
        cfg << "LabelDeclaration=" << labelDeclaration << std::endl;

        size_t index = 0;
        for(auto it=MRU.begin(); it!=MRU.end(); ++it) {
            cfg << "MRU." << index ++ << "=" << *it << std::endl;
        }

        cfg.close();
    }

    void LoadConfig() {
        std::ifstream cfg(configFile, std::ios::binary);
        if(!cfg.is_open()) return;

        for (std::string line; std::getline(cfg, line); ) {
            std::string v = trim(line);
            std::size_t firstEqual = v.find('=');
            if(firstEqual==std::string::npos) continue; // skip invalid line

            std::string key = v.substr(0,firstEqual);
            std::string val = v.substr(firstEqual+1);

            if(key.substr(0,4) == "MRU.") {
                MRU.emplace_back(val);
            } else if(key == "ExporWithComments") {
                exporWithComments = val == "True";
            } else if(key == "ShiftRollingAround") {
                shiftRollingAround = val == "True";
            } else if(key == "LineCommentSymbol") {
                strncpy(lineCommentSymbol, val.c_str(), sizeof(lineCommentSymbol));
            } else if(key == "ByteArrayType") {
                strncpy(byteArrayType, val.c_str(), sizeof(byteArrayType));
            } else if(key == "ConstantDeclaration") {
                strncpy(constantDeclaration, val.c_str(), sizeof(constantDeclaration));
            } else if(key == "LabelDeclaration") {
                strncpy(labelDeclaration, val.c_str(), sizeof(labelDeclaration));
            }
        }

        cfg.close();
    }
};