#pragma once
#include <iostream>
#include <cstring>
#include <stddef.h>
#include <cstdint>
#include <malloc.h>
#include <cassert>
#include <SDL_render.h>

//#define USE_TEST_SPRITE

static int nextID = 0;

struct Sprite
{
    int ID;
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
        High8Frames = 0,
        Medium4Frames = 1,
        Low2Frames = 2,
    };

    // Metdata
    size_t heightInPixels, widthInBytes;
    char description[256] = {0};
    char spriteID[128] = {0};
    ByteAligment byteAligment = ByteAligment::Vertical_Software_Sprite;
    bool multicolorMode = true;
    bool prerenderSoftwareSprite = true;
    PrerendingPrecision renderingPrecision = PrerendingPrecision::Low2Frames;
    ImU32 backgroundColor = 0xff000000; // 0b00
    ImU32 multi1Color     = 0xffff0000; // 0b01
    ImU32 multi2Color     = 0xff0000ff; // 0b10
    ImU32 characterColor  = 0xffffffff; // 0b11
    PaletteType palette = PaletteType::C64_Pal;
    size_t zoomIndex = 3; // 100%
    bool dirty = true;

    const size_t pitch = 64;
    char data[4096]; // 64x64 the maximum size

#ifdef USE_TEST_SPRITE
    char USE_test_sprite[32][16] = {
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
        {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0}
    };
#endif
    Sprite(std::size_t widthInBytes, std::size_t heightInPixels, const char *imageName) : heightInPixels(heightInPixels), widthInBytes(widthInBytes)
    {
        ID = nextID++;
        assert(widthInBytes<=8);
        assert(heightInPixels<=64);
        strncpy(spriteID, imageName, sizeof(spriteID));
        memset((void*)data, 0, sizeof(data));

#ifdef USE_TEST_SPRITE
        size_t widthInPixels = widthInBytes<<3;
        for(size_t y=0;y<heightInPixels;y++) {
            for(size_t x=0;x<widthInPixels;x++) {
                data[y*pitch+x]=USE_test_sprite[y][x];
            }
        }
#endif
    }

    ~Sprite()
    {
        Free();
    }

    void Free()
    {
        #if 0
        std::cerr << "[free] Sprite id " << spriteID << " (" << ID << ")" << std::endl;
        #endif

        if(original) {
            SDL_DestroyTexture(original);
            original = nullptr;
        }
        if(scaled) {
            SDL_DestroyTexture(scaled);
            scaled = nullptr;
        }
    }

    void SetPixel(size_t position_x, size_t position_y, size_t val) {
        if(multicolorMode) {
            data[position_y * pitch + 2*position_x+0] = val>>1;
            data[position_y * pitch + 2*position_x+1] = val&1;
        } else {
            data[position_y * pitch + position_x] = val;
        }
    }

    void SetDrawColor(SDL_Renderer *renderer, ImU32 color) {
        char r = ABGR_RED(color),g=ABGR_GREEN(color),b=ABGR_BLUE(color);
        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    }

    void Invalidate() {
        dirty = true;
    }

    SDL_Texture *GetTexture(SDL_Renderer *renderer, float scale = 1.0f) {

        bool was_dirty = CreateOriginalSizeTextureCache(renderer);
        if(scale == 1.0f) {
            return original;
        }

        if(was_dirty || (scaled == nullptr)) {

            assert(scale>=1.0);
            assert(scale<=8.0);

            // Generated scaled texture
            int w=(int)(widthInBytes<<3), h=(int)(heightInPixels);
            SDL_Rect d = {0,0,(int)(w*scale),(int)(h*scale)};

            if(scaled) {
                SDL_DestroyTexture(scaled);
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

        bool was_dirty = CreateOriginalSizeTextureCache(renderer);
        if(size.x == (widthInBytes<<3) && size.y == heightInPixels) {
            return original;
        }

        if(was_dirty || (scaled == nullptr)) {
            // Generated scaled texture
            SDL_Rect s = {0,0,((int)(widthInBytes<<3)),(int)heightInPixels}, d = {0,0,(int)(size.x),(int)(size.y)};

            if(scaled) {
                SDL_DestroyTexture(scaled);
            }

            scaled = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC|SDL_TEXTUREACCESS_TARGET, d.w, d.h);
            assert(scaled != nullptr);

            assert(SDL_SetRenderTarget(renderer, scaled)==0);
            SDL_RenderCopy(renderer, original, &s, &d);
            assert(SDL_SetRenderTarget(renderer, nullptr)==0);
        }
        return scaled;
    }

    bool CreateOriginalSizeTextureCache(SDL_Renderer *renderer) {
        bool was_dirty = dirty;
        if(dirty || (original == nullptr)) {
            dirty = false;
            was_dirty = true;

            if(original == nullptr) {
                original = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC|SDL_TEXTUREACCESS_TARGET, 64, 64);
            }

            // Generated original size texture
            assert(SDL_SetRenderTarget(renderer, original)==0);

            SDL_SetRenderDrawColor(renderer, ABGR_RED(backgroundColor), ABGR_GREEN(backgroundColor), ABGR_BLUE(backgroundColor), SDL_ALPHA_OPAQUE);
            SDL_RenderClear(renderer);

            size_t widthInPixels = (widthInBytes<<3);
            for (size_t j = 0; j < heightInPixels; ++j) {
                if (multicolorMode) {
                    for (size_t i = 0; i < widthInPixels; i += 2) {
                        char *p = (char*)data + i + j*pitch;
                        size_t v = (p[0] << 1) | p[1];
                        if(v == 0) continue; // skip background rendering (we already cleared the whole background with one call)
                        switch (v) {
                        case 1: SetDrawColor(renderer, multi1Color); break;
                        case 2: SetDrawColor(renderer, multi2Color); break;
                        case 3: SetDrawColor(renderer, characterColor); break;
                        }
                        SDL_Rect r = {(int)i, (int)j, 2, 1};
                        SDL_RenderFillRect(renderer, &r);
                    }
                } else {
                    for (size_t i = 0; i < widthInPixels; ++i) {
                        char *p = (char*)data + i + j*pitch;
                        if (*p) {
                            SetDrawColor(renderer, characterColor);
                            SDL_Rect r = {(int)i, (int)j, 1, 1};
                            SDL_RenderFillRect(renderer, &r);
                        }
                    }
                }
            }
            assert(SDL_SetRenderTarget(renderer, nullptr)==0);
        }
        return was_dirty;
    }

    size_t GetByteIndex(size_t row, size_t col) {
        if(multicolorMode) col *= 2;
        switch(byteAligment) {
            case ByteAligment::Horizontal_C64_Sprite:       return row*widthInBytes+(size_t)(col/8);
            case ByteAligment::Vertical_Software_Sprite:    return row+(size_t)(col/8)*heightInPixels;
            case ByteAligment::Mixed_Character_Based:       return row%8+(size_t)(col/8)*8+(size_t)(row/8)*widthInBytes*8;
        }
        return 0;
    }

    size_t GetCharIndex(size_t row, size_t col) {
        if(byteAligment != ByteAligment::Mixed_Character_Based) return 64;
        return (size_t)(GetByteIndex(row, col)/8);
    }

    size_t GetByteSize() {
        size_t sz = (byteAligment == ByteAligment::Mixed_Character_Based ? (size_t)(((size_t)(heightInPixels+7)/8))*8 : heightInPixels);
        size_t size = widthInBytes*sz;
        if(prerenderSoftwareSprite) {
            size = (widthInBytes+1)*sz;
            switch(renderingPrecision) {
                case PrerendingPrecision::Low2Frames:    size *= 2; break;
                case PrerendingPrecision::Medium4Frames: size *= 4; break;
                case PrerendingPrecision::High8Frames:   size *= 8; break;
            }
        }
        return size;
    }

    std::string GetByteAlignment() {
        switch(byteAligment) {
            case ByteAligment::Horizontal_C64_Sprite: return "Horizontal";
            case ByteAligment::Vertical_Software_Sprite: return "Vertical";
            case ByteAligment::Mixed_Character_Based: return "Mixed";
        }
        return "<GetByteAlignment: error>";
    }

    ByteAligment GetByteAlignment(const std::string &str) {
        if(str == "Horizontal") return ByteAligment::Horizontal_C64_Sprite;
        if(str == "Vertical") return ByteAligment::Vertical_Software_Sprite;
        if(str == "Mixed") return ByteAligment::Mixed_Character_Based;
        return ByteAligment::Horizontal_C64_Sprite;
    }

    std::string GetRenderingPrecision() {
        switch(renderingPrecision) {
            case PrerendingPrecision::Low2Frames: return "Low2Frames";
            case PrerendingPrecision::Medium4Frames: return "Medium4Frames";
            case PrerendingPrecision::High8Frames: return "High8Frames";
        }
        return "<GetRenderingPrecision: error>";
    }

    PrerendingPrecision GetRenderingPrecision(const std::string &str) {
        if(str == "Low2Frames") return PrerendingPrecision::Low2Frames;
        if(str == "Medium4Frames") return PrerendingPrecision::Medium4Frames;
        if(str == "High8Frames") return PrerendingPrecision::High8Frames;
        return PrerendingPrecision::Low2Frames;
    }

    std::string GetPaletteName() {
        switch(palette) {
            case PaletteType::C64_Pal: return "Commodore64";
            case PaletteType::C264_Pal: return "Commodore264Series";
        }
        return "<GetPaletteName: error>";
    }

    PaletteType GetPaletteName(const std::string &str) {
        if(str == "Commodore64") return PaletteType::C64_Pal;
        if(str == "Commodore264Series") return PaletteType::C264_Pal;
        return PaletteType::C64_Pal;
    }
};