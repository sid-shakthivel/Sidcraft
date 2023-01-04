#pragma once

#include "stb_image.h"

unsigned int LoadTextureFromFile(const std::string &filepath)
{
    unsigned int TextureId;
    glGenTextures(1, &TextureId);
    glBindTexture(GL_TEXTURE_2D, TextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);          // (s, t, r) refers to the coordinate axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);          // Mirrors content
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Set filtering method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char *data = stbi_load((filepath).c_str(), &width, &height, &nrChannels, 0);

    if (!data)
        std::cout << "ERROR LOADING TEXTURE\n";

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return TextureId;
}

class TextureAtlas
{
private:
    unsigned int TextureAtlasId;

protected:
    TextureAtlas()
    {
        TextureAtlasId = LoadTextureFromFile("TextureAtlas.png");
    }

    static TextureAtlas *TextureAtlasSingleton_;

public:
    TextureAtlas(TextureAtlas &other) = delete;    // Singletons shouldn't be cloneable
    void operator=(const TextureAtlas &) = delete; // Singletons shouldn't be assignable
    static TextureAtlas *GetInstance();

    unsigned int GetTextureAtlasId()
    {
        return TextureAtlasId;
    }

    float FetchGrassTop()
    {
        return 0.0f;
    }

    float FetchGrassSide()
    {
        return 1.0f;
    }

    float FetchTreeTrunk()
    {
        return 4.0f;
    }

    float FetchTreeLeaves()
    {
        return 6.0f;
    }
};

TextureAtlas *TextureAtlas::TextureAtlasSingleton_ = nullptr;

TextureAtlas *TextureAtlas::GetInstance()
{
    if (TextureAtlasSingleton_ == nullptr)
        TextureAtlasSingleton_ = new TextureAtlas();

    return TextureAtlasSingleton_;
}
