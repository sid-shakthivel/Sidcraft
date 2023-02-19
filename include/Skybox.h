#pragma once

#include <vector>

#include "Matrix.h"
#include "Shader.h"

class Skybox
{
private:
    std::vector<const char *> LightImagePaths = {
        "res/SkyBoxImages/right.png",
        "res/SkyBoxImages/left.png",
        "res/SkyBoxImages/top.png",
        "res/SkyBoxImages/bottom.png",
        "res/SkyBoxImages/back.png",
        "res/SkyBoxImages/front.png",
    };

    std::vector<const char *> DarkImagePaths = {
        "res/DarkSkyBoxImages/right.png",
        "res/DarkSkyBoxImages/left.png",
        "res/DarkSkyBoxImages/top.png",
        "res/DarkSkyBoxImages/bottom.png",
        "res/DarkSkyBoxImages/back.png",
        "res/DarkSkyBoxImages/front.png",
    };

    unsigned int VAO, VBO, EBO;
    unsigned int LightTextureId;
    unsigned int DarkTextureId;

    static constexpr float SIZE = 500.0f;

    static constexpr float SkyboxVertices[] =
        {
            //   Coordinates
            -SIZE, -SIZE, SIZE,
            SIZE, -SIZE, SIZE,
            SIZE, -SIZE, -SIZE,
            -SIZE, -SIZE, -SIZE,
            -SIZE, SIZE, SIZE,
            SIZE, SIZE, SIZE,
            SIZE, SIZE, -SIZE,
            -SIZE, SIZE, -SIZE};

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

    float RotationAngle;
    Matrix4f ModelMatrix;

    static constexpr float SPEED = 3.0f;
    float TotalTime = 0.0f;
    float BlendFactor = 0.0f;

    void UpdateBlend(float DeltaTime, Shader *MeshShader);

public:
    Skybox();
    void Draw(Shader *MeshShader, float DeltaTime);
};