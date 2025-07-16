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
    size_t dataHash;

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

    struct CacheData {
        size_t hash;
        SDL_Texture *tx;
    };

    std::map<size_t, CacheData> resizedCache;

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
    size_t charOffset, charIndex;

    struct Frame {
        bool dirty;
        SDL_Texture *org;
        std::map<size_t, CacheData> cache;
        char data[4096]; // 64x64 the maximum size
    };

    bool animationAttached = false;
    int animationFPS = 4;
    std::vector<Frame> animationFrames;

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
    Sprite() {}
    Sprite(std::size_t widthInBytes, std::size_t heightInPixels, const char *imageName) : heightInPixels(heightInPixels), widthInBytes(widthInBytes)
    {
        ID = nextID++;
        assert(widthInBytes<=8);
        assert(heightInPixels<=64);
        strncpy(spriteID, imageName, sizeof(spriteID));
        ClearData();

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
        if(!resizedCache.empty()) {
            std::for_each(resizedCache.begin(), resizedCache.end(), [](const std::pair<size_t, CacheData>& p) { SDL_DestroyTexture(p.second.tx); });
        }
        std::for_each(animationFrames.begin(), animationFrames.end(), [](Frame& f) {
            SDL_DestroyTexture(f.org);
            if(!f.cache.empty()) {
                std::for_each(f.cache.begin(), f.cache.end(), [](const std::pair<size_t, CacheData>& p) { SDL_DestroyTexture(p.second.tx); });
            }
        });
    }

    void ClearData() {
        memset((void*)data, 0, sizeof(data));
        dataHash = std::hash<std::string_view>()(std::string_view(data, sizeof(data)));
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
        dataHash = std::hash<std::string_view>()(std::string_view(data, sizeof(data)));
    }

    size_t GetHash() {
        return GetHash(dataHash);
    }

    size_t GetHash(size_t dHash) {
        size_t hash = 7;
        hash = hash * 31 + dHash;
        hash = hash * 31 + (multicolorMode ? 1 : 0);
        hash = hash * 31 + backgroundColor;
        hash = hash * 31 + characterColor;
        hash = hash * 31 + multi1Color;
        hash = hash * 31 + multi2Color;
        return hash;
    }

    size_t GetSizeHash(ImVec2 size) {
        size_t hash = 7;
        hash = (int)((float)hash * 31 + size.x);
        hash = (int)((float)hash * 31 + size.y);
        return hash;
    }

    SDL_Texture *GetTextureFixedSize(SDL_Renderer *renderer, const ImVec2 size) {
        return GetTextureFixedSize(renderer, size, &original, resizedCache, data, &dirty);
    }

    SDL_Texture *GetTextureFixedSize(SDL_Renderer *renderer, const ImVec2 size, SDL_Texture **texture, std::map<size_t, CacheData>& rescache, const char *d, bool* pDirty) {

        if(*pDirty) {
            CreateOriginalSizeTextureCache(renderer, texture, d);
            *pDirty = false;
        }

        if(size.x == (widthInBytes<<3) && size.y == heightInPixels) {
            return *texture;
        }

        size_t sizeHash = GetSizeHash(size);
        size_t h = std::hash<std::string_view>()(std::string_view(d, sizeof(*d)));
        size_t spriteHash = GetHash(h);

        auto cache = rescache.find(sizeHash);
        CacheData* resized = cache == rescache.end() ? nullptr : &(*cache).second;
        if(!resized) {
            auto it = rescache.insert({sizeHash, {h, nullptr}});
            resized = &it.first->second;
        }

        if(resized->hash != spriteHash || (resized->tx == nullptr)) {
            // Generated scaled texture
            SDL_Rect s = {0,0,((int)(widthInBytes<<3)),(int)heightInPixels}, d = {0,0,(int)(size.x),(int)(size.y)};

            if(resized->tx == nullptr) {
                resized->tx = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC|SDL_TEXTUREACCESS_TARGET, d.w, d.h);
                assert(resized->tx != nullptr);
            }

            assert(SDL_SetRenderTarget(renderer, resized->tx)==0);
            SDL_RenderCopy(renderer, *texture, &s, &d);
            assert(SDL_SetRenderTarget(renderer, nullptr)==0);

            resized->hash = spriteHash;
        }
        return resized->tx;
    }

    void CreateOriginalSizeTextureCache(SDL_Renderer *renderer) {
        CreateOriginalSizeTextureCache(renderer, &original, data);
    }

    void CreateOriginalSizeTextureCache(SDL_Renderer *renderer, SDL_Texture **texture, const char *d) {

        if(*texture == nullptr) {
            *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC|SDL_TEXTUREACCESS_TARGET, 64, 64);
        }

        // Generated original size texture
        assert(SDL_SetRenderTarget(renderer, *texture)==0);

        SDL_SetRenderDrawColor(renderer, ABGR_RED(backgroundColor), ABGR_GREEN(backgroundColor), ABGR_BLUE(backgroundColor), SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        size_t widthInPixels = (widthInBytes<<3);
        for (size_t j = 0; j < heightInPixels; ++j) {
            if (multicolorMode) {
                for (size_t i = 0; i < widthInPixels; i += 2) {
                    char *p = (char*)d + i + j*pitch;
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
                    char *p = (char*)d + i + j*pitch;
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
        assert(true); // unreachable
        return nullptr;
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
        assert(true); // unreachable
        return nullptr;
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
        assert(true); // unreachable
        return nullptr;
    }

    PaletteType GetPaletteName(const std::string &str) {
        if(str == "Commodore64") return PaletteType::C64_Pal;
        if(str == "Commodore264Series") return PaletteType::C264_Pal;
        return PaletteType::C64_Pal;
    }
};
