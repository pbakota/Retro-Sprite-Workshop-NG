#pragma once
#include <vector>
#include <memory>
#include "sprite.h"
#include "project.h"

struct Project;

struct SpriteManager {
    Project *project;

    std::vector<Sprite*> sprites;
    std::string projectFile = "";

    SpriteManager(Project *project): project(project) { }

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

    bool LoadProject(const std::string& filename) {
        std::vector<Sprite*> temp;
        bool success = (project->Load(filename, temp));
        if(success) {
            sprites = temp;
        }

        return success;
    }

    bool SaveProject(const std::string& filename) {
        return project->Save(filename, sprites);
    }
};