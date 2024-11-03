#pragma once
#include <iostream>
#include <cstring>
#include <stddef.h>
#include <cstdint>
#include <malloc.h>
#include <cassert>
#include <SDL_render.h>

static int nextID = 0;

struct Sprite
{
    int ID;
    SDL_Surface *surface;
    SDL_Texture *original = nullptr, *scaled = nullptr;

    enum class PaletteType {
        C64_Pal = 0,
        C264_Pal = 1,
    };

    enum class ByteAligment
    {
        Horizontal_C64_Sprite = 0,
        Vertical_Software_Sprite = 1,
        Mixed_Character_Based = 2,
    };

    enum class PrerendingPrecision {
        Low2Frames = 0,
        Medium4Frames = 1,
        High8Frames = 2,
    };

    // Metdata
    size_t heightInPixels, widthInBytes;
    char description[256] = {0};
    char spriteID[128] = {0};
    ByteAligment byteAligment = ByteAligment::Vertical_Software_Sprite;
    bool multicolorMode = true;
    bool prerenderSoftwareSprite = true;
    PrerendingPrecision renderingPrecision = PrerendingPrecision::Low2Frames;
    ImU32 backgroundColor;
    ImU32 characterColor;
    ImU32 multi1Color;
    ImU32 multi2Color;
    PaletteType palette;
    bool dirty = true;

    Sprite(std::size_t widthInBytes, std::size_t heightInPixels, const char *imageName) : heightInPixels(heightInPixels), widthInBytes(widthInBytes)
    {
        ID = nextID++;

        strncpy(spriteID, imageName, sizeof(spriteID));
        surface = SDL_CreateRGBSurface(0, 64, 64, 32, 0, 0, 0, 0);
        assert(surface != nullptr);
        SDL_FillRect(surface, nullptr, 0xffaaaaaa);
    }

    ~Sprite()
    {
        Free();
    }

    void Free()
    {
        if(surface) {
            SDL_FreeSurface(surface);
            surface = nullptr;
        }
        if(original) {
            SDL_DestroyTexture(original);
            original = nullptr;
        }
        if(scaled) {
            SDL_DestroyTexture(scaled);
            scaled = nullptr;
        }
    }

    SDL_Texture *GetTexture(SDL_Renderer *renderer, float scale = 1.0f)
    {
        if(original && !dirty && ((scale <= 1.0f || scale > 8.0f) || !SDL_RenderTargetSupported(renderer))) {
            return original;
        }

        if(dirty || (original == nullptr)) {
            dirty = false;

            if(original) {
                SDL_DestroyTexture(original);
            }

            // Generated original size texture
            original = SDL_CreateTextureFromSurface(renderer, surface);
            if(scale == 1.0f) {
                return original;
            }

            // Generated scaled texture
            int w=(int)(widthInBytes<<3), h=(int)(heightInPixels);
            SDL_Rect d = {0,0,(int)(w*scale),(int)(h*scale)};

            if(scaled) {
                SDL_DestroyTexture(scaled);
                scaled = nullptr;
            }

            scaled = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC|SDL_TEXTUREACCESS_TARGET, d.w, d.h);
            assert(scaled != nullptr);

            assert(SDL_SetRenderTarget(renderer, scaled)==0);
            SDL_RenderCopy(renderer, original, nullptr, &d);
            assert(SDL_SetRenderTarget(renderer, nullptr)==0);
        }
        return scaled;
    }

    SDL_Texture *GetTextureFixedSize(SDL_Renderer *renderer, ImVec2 size) {
        if(original && (!dirty || !SDL_RenderTargetSupported(renderer))) {
            return original;
        }

        if(dirty || (original == nullptr)) {
            dirty = false;

            if(original) {
                SDL_DestroyTexture(original);
            }

            // Generated original size texture
            original = SDL_CreateTextureFromSurface(renderer, surface);

            // Generated scaled texture
            SDL_Rect d = {0,0,(int)(size.x),(int)(size.y)};

            if(scaled) {
                SDL_DestroyTexture(scaled);
                scaled = nullptr;
            }

            scaled = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC|SDL_TEXTUREACCESS_TARGET, d.w, d.h);
            assert(scaled != nullptr);

            assert(SDL_SetRenderTarget(renderer, scaled)==0);
            SDL_RenderCopy(renderer, original, nullptr, &d);
            assert(SDL_SetRenderTarget(renderer, nullptr)==0);
        }
        return scaled;
    }

    void SetPixel(size_t x, size_t y, /*ABGR*/ ImU32 color) {
        if(x>=(multicolorMode ? (widthInBytes<<2) : (widthInBytes<<3))&&(y>=heightInPixels)) return;
        dirty = true;
        if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
        if(multicolorMode) {
            ((Uint32*)surface->pixels)[y*surface->pitch + x+0] = color;
            ((Uint32*)surface->pixels)[y*surface->pitch + x+1] = color;
        } else {
            ((Uint32*)surface->pixels)[y*surface->pitch + x+0] = color;
        }
        if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    }

    std::string GetByteAlignment() {
        switch(byteAligment) {
            case ByteAligment::Horizontal_C64_Sprite: return "Horizontal";
            case ByteAligment::Vertical_Software_Sprite: return "Vertical";
            case ByteAligment::Mixed_Character_Based: return "Mixed";
        }
        return "<GetByteAlignment: error>";
    }

    std::string GetRenderingPrecision() {
        switch(renderingPrecision) {
            case PrerendingPrecision::Low2Frames: return "Low2Frames";
            case PrerendingPrecision::Medium4Frames: return "Medium4Frames";
            case PrerendingPrecision::High8Frames: return "High8Frames";
        }
        return "<GetRenderingPrecision: error>";
    }
};