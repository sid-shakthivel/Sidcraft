#pragma once

#include "Mesh.h"

class Quad : public Mesh
{
private:
    // static constexpr float QuadData[] = {
    //     -1.0f,
    //     1.0f,
    //     0.0f,
    //     0.0f,
    //     1.0f,
    //     -1.0f,
    //     -1.0f,
    //     0.0f,
    //     0.0f,
    //     0.0f,
    //     1.0f,
    //     1.0f,
    //     0.0f,
    //     1.0f,
    //     1.0f,
    //     1.0f,
    //     -1.0f,
    //     0.0f,
    //     1.0f,
    //     0.0f,
    // };

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

public:
    Quad();
    void CreateMesh();
    void Draw();
};