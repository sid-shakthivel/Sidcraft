#pragma once

#include "Mesh.h"

class Quad : public Mesh
{
private:
    static constexpr float NewQuadData[]{
        -1.0f,
        1.0f,
        0.0f,

        0.0f,
        1.0f,

        -1.0f,
        -1.0f,
        0.0f,

        0.0f,
        0.0f,

        1.0f,
        -1.0f,
        0.0f,

        1.0f,
        0.0f,

        1.0f,
        1.0f,
        0.0f,

        1.0f,
        1.0f,
    };

    static constexpr unsigned int Indices[]{
        0, 2, 1, 2, 0, 3};

    Matrix4f ModelMatrix = Matrix4f(1);

public:
    Quad(Vector3f ScaleFactor = Vector3f(1.0f, 1.0f, 1.0f));
    void CreateMesh();
    void Draw(Shader *QuadShader);
};