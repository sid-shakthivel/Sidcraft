#pragma once

#include "./Block.h"
#include "./Matrix.h"
#include "./Cube.h"

unsigned int LoadTextureFromRGBA(const std::string &filepath);
unsigned int LoadTextureFromRGB(const std::string &filepath);
unsigned int LoadTexuresForCubemap(std::vector<const char *> ImagePaths);

class TextureAtlas
{
private:
    unsigned int TextureAtlasId;

protected:
    TextureAtlas();
    static TextureAtlas *TextureAtlasSingleton_;

public:
    TextureAtlas(TextureAtlas &other) = delete;    // Singletons shouldn't be cloneable
    void operator=(const TextureAtlas &) = delete; // Singletons shouldn't be assignable
    static TextureAtlas *GetInstance();

    unsigned int GetTextureAtlasId();
    float FetchTexture(BlockType Block, Vector3f Direction = Vector3f(0.0f, 0.0f, 0.0f));
};

inline TextureAtlas *TextureAtlas::TextureAtlasSingleton_ = nullptr;