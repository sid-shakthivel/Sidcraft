#pragma once

#include <vector>

#include "Matrix.h"
#include "Shader.h"

class Skybox
{
private:
    std::vector<const char *> ImagePaths = {
        "res/SkyBoxImages/right.png",
        "res/SkyBoxImages/left.png",
        "res/SkyBoxImages/top.png",
        "res/SkyBoxImages/bottom.png",
        "res/SkyBoxImages/back.png",
        "res/SkyBoxImages/front.png",
    };

    unsigned int VAO, VBO, EBO;
    unsigned int TextureId;

    static constexpr float SkyboxVertices[] =
        {
            //   Coordinates
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f};

    static constexpr unsigned int SkyboxIndices[] =
        {
            // Right
            1, 2, 6,
            6, 5, 1,
            // Left
            0, 4, 7,
            7, 3, 0,
            // Top
            4, 5, 6,
            6, 7, 4,
            // Bottom
            0, 3, 2,
            2, 1, 0,
            // Back
            0, 1, 5,
            5, 4, 0,
            // Front
            3, 7, 6,
            6, 2, 3};

public:
    Skybox();
    void Draw(Shader *MeshShader);
};