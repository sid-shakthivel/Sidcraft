#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../include/stb_image.h"

#include "../include/TextureAtlas.h"

unsigned int LoadTextureFromFile(const std::string &filepath)
{
    unsigned int TextureId;
    glGenTextures(1, &TextureId);
    glBindTexture(GL_TEXTURE_2D, TextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);           // (s, t, r) refers to the coordinate axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);           // Mirrors content
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); // Set filtering method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char *data = stbi_load((filepath).c_str(), &width, &height, &nrChannels, 0);

    if (!data)
        std::cout << "ERROR LOADING TEXTURE\n";

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return TextureId;
}

unsigned int LoadRBGFromFile(const std::string &filepath)
{
    unsigned int TextureId;
    glGenTextures(1, &TextureId);
    glBindTexture(GL_TEXTURE_2D, TextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);                    // (s, t, r) refers to the coordinate axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                    // Mirrors content
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); // Set filtering method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char *data = stbi_load((filepath).c_str(), &width, &height, &nrChannels, 0);

    if (!data)
        std::cout << "ERROR LOADING TEXTURE\n";

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return TextureId;
}

TextureAtlas *TextureAtlas::GetInstance()
{
    if (TextureAtlasSingleton_ == nullptr)
        TextureAtlasSingleton_ = new TextureAtlas();

    return TextureAtlasSingleton_;
}

TextureAtlas::TextureAtlas()
{
    TextureAtlasId = LoadTextureFromFile("res/TextureAtlas.png");
}

unsigned int TextureAtlas::GetTextureAtlasId()
{
    return TextureAtlasId;
}

float TextureAtlas::FetchGrassTop()
{
    return 240.0f;
}

float TextureAtlas::FetchGrassSide()
{
    return 241.0f;
}

float TextureAtlas::FetchDirt()
{
    return 242.0f;
}

float TextureAtlas::FetchStone()
{
    return 243.0f;
}

float TextureAtlas::FetchTreeTrunk()
{
    return 244.0f;
}

float TextureAtlas::FetchTreeLeaves()
{
    return 246.0f;
}

float TextureAtlas::FetchSand()
{
    return 247.0f;
}

float TextureAtlas::FetchWater()
{
    return 248.0f;
}

float TextureAtlas::FetchFlower()
{
    return 250.0f;
}

float TextureAtlas::FetchTallGrass()
{
    return 251.0f;
}