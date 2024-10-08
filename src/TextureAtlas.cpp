#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../include/stb_image.h"
#include <vector>

#include "../include/Vector.h"
#include "../include/TextureAtlas.h"

unsigned int LoadTexuresForCubemap(std::vector<const char *> ImagePaths)
{
    unsigned int TextureId;

    glGenTextures(1, &TextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);

    int Width, Height, NrChannels;
    unsigned char *Data;

    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < ImagePaths.size(); i++)
    {
        Data = stbi_load(ImagePaths[i], &Width, &Height, &NrChannels, 0);
        if (!Data)
            std::cout << "ERROR: LOADING TEXTURE" << std::endl;
        else
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
            stbi_image_free(Data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return TextureId;
}

unsigned int LoadTextureFromRGBA(const std::string &filepath)
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

unsigned int LoadTextureFromRGB(const std::string &filepath)
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
    TextureAtlasId = LoadTextureFromRGBA("../res/TextureAtlas.png");
}

unsigned int TextureAtlas::GetTextureAtlasId()
{
    return TextureAtlasId;
}

float TextureAtlas::FetchTexture(BlockType Block, Vector3f Direction)
{
    switch (Block)
    {
    case BlockType::Grass:
        return (Direction.IsEqual(Cube::DirectionList[0]) || Direction.IsEqual(Cube::DirectionList[1])) ? 240 : 241;
    case BlockType::Dirt:
        return 242;
    case BlockType::Stone:
        return 243;
    case BlockType::TreeTrunk:
        return 244;
    case BlockType::TreeLeaves:
        return 246;
    case BlockType::Sand:
        return 247;
    case BlockType::Water:
        return 248;
    case BlockType::Flower:
        return 250;
    case BlockType::TallGrass:
        return 251;
    default:
        std::cout << "ERROR: Unknown Block Type" << std::endl;
        std::exit(0);
    }
}