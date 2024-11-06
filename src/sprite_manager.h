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
        Clear();
    }

    void Clear() {
        for (auto it = sprites.begin(); it != sprites.end(); ++it) {
            delete *it;
        }
        sprites.clear();
    }

    void NewProject() {
        project->NewProject();
        DetachSprite();
        Clear();
        // Always append one (default) sprite
        NewSprite();
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
            Clear();
            sprites = temp;
        }

        return result;
    }

    bool SaveProject(const std::string& filename) {
        return project->Save(filename, sprites);
    }
};