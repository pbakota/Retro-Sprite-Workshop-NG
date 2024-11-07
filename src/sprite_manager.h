#pragma once
#include <vector>
#include <algorithm>
#include <memory>
#include "sprite.h"
#include "project.h"
#include "sprite_image.h"
#include "statusbar.h"

struct SpriteManager {
    Project *project;
    SpriteImage *spriteImage;
    StatusBar *statusbar;

    std::vector<Sprite*> sprites;
    std::string projectFile = "";
    int spriteListType = 0;

    SpriteManager(Project *project, SpriteImage *spriteImage, StatusBar* statusbar) : project(project), spriteImage(spriteImage), statusbar(statusbar) { }

    ~SpriteManager() {
        ClearSpriteList();
    }

    void ClearSpriteList() {
        for (auto it = sprites.begin(); it != sprites.end(); ++it) {
            delete *it;
        }
        sprites.clear();
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
        auto current = find_if(sprites.begin(), sprites.end(), [id](auto&&sp) { return (sp->ID == id); });
        Sprite *sp = *current;
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

    void NewProject() {
        DetachSprite();
        ClearSpriteList();
        // Always append one (default) sprite
        NewSprite();
        project->NewProject();
    }

    void NewSprite() {
        Sprite* sprite = new Sprite(1,8, "sprite");
        sprite->byteAligment = Sprite::ByteAligment::Mixed_Character_Based;
        sprite->multicolorMode = false;
        sprite->prerenderSoftwareSprite = false;
        sprite->renderingPrecision = Sprite::PrerendingPrecision::Medium4Frames;
        // TODO: Copy last used colors into the new sprite

        AppendSprite(sprite);
    }

    // Append new sprite at the end of the list
    void AppendSprite(Sprite *sprite) {
        sprites.push_back(sprite);
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
        spriteImage->currentSprite = sprite;
        statusbar->is_zoom_visible = true;
        statusbar->zoomIndex = sprite->zoomIndex;
    }

    void DetachSprite() {
        spriteImage->currentSprite = nullptr;
        statusbar->is_zoom_visible = false;
    }

    bool LoadProject(const std::string& filename) {
        std::vector<Sprite*> temp;
        bool result = project->Load(filename, temp);
        if(result) {
            DetachSprite();
            ClearSpriteList();
            sprites = temp;
        }

        return result;
    }

    bool SaveProject() {
        return project->Save(projectFile, sprites);
    }

    bool SaveProjectAs(const std::string& filename) {
        if(project->Save(filename, sprites)) {
            projectFile = filename;
            return true;
        }
        return false;
    }
};