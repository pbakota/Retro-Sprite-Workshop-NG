#pragma once
#include <vector>
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
        Clear();
        Sprite* sprite = new Sprite(1,8, "sprite");
        sprite->byteAligment = Sprite::ByteAligment::Vertical_Software_Sprite;
        sprite->multicolorMode = false;
        sprite->prerenderSoftwareSprite = false;
        sprite->renderingPrecision = Sprite::PrerendingPrecision::Low2Frames;
        AppendSprite(sprite);
    }

    // Append new sprite at the end of the list
    void AppendSprite(Sprite *sprite) {
        sprites.push_back(sprite);
    }

    // Remove sprite at index
    void RemoveSprite(const std::size_t index){}

    // Insert sprite after index
    void InsertSprite(const std::size_t index, const Sprite &sprite) {}

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