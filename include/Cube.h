#include "Matrix.h"
#include "Mesh.h"

#include <array>
#include <map>

#pragma once

class Cube : public Mesh
{
public:
    Cube(float TextureIndex = 0, Vector3f TranslationVector = Vector3f(0.0f, 0.0f, 0.0f));
    ~Cube() override = default;

    // Important cube data
    static std::array<unsigned int, 6> FaceIndices;
    static std::array<Vector3f, 6> DirectionList;
    static std::array<Vector3f, 6> FaceNormals;
    static std::array<std::array<Vector3f, 4>, 6> FaceVertices;
    static std::vector<Vector2f> TextureCoordinatesList;

    static unsigned int ConvertDirectionToNumber(Vector3f Direction);
};