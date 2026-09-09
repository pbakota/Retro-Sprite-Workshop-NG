#pragma once

#include <string>
#include <deque>
#include <cstring>
#include <SDL_render.h>
#include "sprite.h"
#include "serializator.h"

extern SDL_Renderer *renderer;

// A simple snapshot-based undo/redo history for a single sprite.
// Snapshots are serialized (text) so they do not hold SDL_Texture pointers.
// The history is scoped to the sprite the entries were recorded for; entries
// recorded for other sprites are dropped so a snapshot can never be applied to
// the wrong sprite.
struct UndoRedoHistory {
    static const size_t MAX_UNDO = 50;

    struct Entry {
        int spriteID;
        std::string data;
    };

    std::deque<Entry> undoStack;
    std::deque<Entry> redoStack;

    bool CanUndo(Sprite *sp) const { return sp && !undoStack.empty() && undoStack.back().spriteID == sp->ID; }
    bool CanRedo(Sprite *sp) const { return sp && !redoStack.empty() && redoStack.back().spriteID == sp->ID; }

    void Clear() {
        undoStack.clear();
        redoStack.clear();
    }

    // Record the current state of the sprite as an undo point.
    void Record(Sprite *sp) {
        // If the top of the stack belongs to another sprite, drop it: the
        // history is meant to track a single (the current) sprite.
        if(!undoStack.empty() && undoStack.back().spriteID != sp->ID) {
            undoStack.clear();
        }
        undoStack.push_back({sp->ID, Serializator::Serialize(sp, false)});
        if(undoStack.size() > MAX_UNDO) {
            undoStack.pop_front();
        }
        redoStack.clear();
    }

    // Undo: push current state onto redo, restore last undo snapshot.
    bool Undo(Sprite *sp) {
        if(!sp || undoStack.empty()) return false;
        if(undoStack.back().spriteID != sp->ID) return false;
        redoStack.push_back({sp->ID, Serializator::Serialize(sp, false)});
        std::string snapshot = undoStack.back().data;
        undoStack.pop_back();
        ApplySnapshot(sp, snapshot);
        return true;
    }

    // Redo: push current state onto undo, restore last redo snapshot.
    bool Redo(Sprite *sp) {
        if(!sp || redoStack.empty()) return false;
        if(redoStack.back().spriteID != sp->ID) return false;
        undoStack.push_back({sp->ID, Serializator::Serialize(sp, false)});
        std::string snapshot = redoStack.back().data;
        redoStack.pop_back();
        ApplySnapshot(sp, snapshot);
        return true;
    }

private:
    // Restore a sprite from a serialized snapshot. Runs into a temporary sprite
    // first so that animation frames can be rebuilt without aliasing texture
    // pointers and without Sprite::Deserialize appending to existing frames.
    void ApplySnapshot(Sprite *sp, const std::string &snapshot) {
        Sprite temp;
        Serializator::Deserialize(snapshot.c_str(), &temp);

        sp->widthInBytes       = temp.widthInBytes;
        sp->heightInPixels     = temp.heightInPixels;
        sp->byteAligment       = temp.byteAligment;
        sp->multicolorMode     = temp.multicolorMode;
        // NOTE: prerenderSoftwareSprite, palette and zoomIndex are NOT
        // serialized in the snapshots, so the temp sprite would hold its
        // class defaults here. Restoring them would reset the user's settings
        // to those defaults on every undo, so they are deliberately left
        // untouched (they are not part of the pixel-edit undo scope).
        sp->renderingPrecision = temp.renderingPrecision;
        sp->backgroundColor    = temp.backgroundColor;
        sp->multi1Color        = temp.multi1Color;
        sp->multi2Color        = temp.multi2Color;
        sp->characterColor     = temp.characterColor;
        sp->masked             = temp.masked;
        strncpy(sp->spriteID, temp.spriteID, sizeof(sp->spriteID));
        strncpy(sp->description, temp.description, sizeof(sp->description));
        memcpy(sp->data, temp.data, sizeof(sp->data));
        // temp.mask is only deserialized for masked sprites; the default Sprite
        // constructor leaves data/mask uninitialized, so zero it explicitly.
        memset(sp->mask, 0, sizeof(sp->mask));
        if(temp.masked) {
            memcpy(sp->mask, temp.mask, sizeof(sp->mask));
        }

        sp->animationAttached  = temp.animationAttached;
        sp->animationFPS       = temp.animationFPS;

        // Rebuild animation frames: free any existing frame textures first
        FreeAnimationFrames(sp);
        if(sp->animationAttached) {
            for(auto &fr : temp.animationFrames) {
                Sprite::Frame nf;
                nf.image = sp->CreateSpriteImageTexture(renderer);
                memcpy(nf.data, fr.data, sizeof(nf.data));
                memcpy(nf.mask, fr.mask, sizeof(nf.mask));
                sp->animationFrames.push_back(nf);
            }
        }

        sp->Invalidate();
    }

    void FreeAnimationFrames(Sprite *sp) {
        for(auto &fr : sp->animationFrames) {
            if(fr.image) {
                SDL_DestroyTexture(fr.image);
                fr.image = nullptr;
            }
        }
        sp->animationFrames.clear();
    }
};