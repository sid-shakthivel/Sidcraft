#pragma once

unsigned int LoadTextureFromFile(const std::string &filepath);

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
    float FetchGrassTop();
    float FetchGrassSide();
    float FetchTreeTrunk();
    float FetchTreeLeaves();
};

inline TextureAtlas *TextureAtlas::TextureAtlasSingleton_ = nullptr;