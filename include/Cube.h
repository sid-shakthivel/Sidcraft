#include "Matrix.h"
#include "Mesh.h"

#include <array>
#include <map>

#pragma once

class Cube
{
private:
    Matrix4f Position = Matrix4f(1);

public:
    Cube(float TextureIndex = 0, Vector3f TranslationVector = Vector3f(0.0f, 0.0f, 0.0f));

    void CreateMesh();
    void Draw(Shader *MeshShader, Matrix4f Offset) const;
    void Draw(Shader *MeshShader);

    // Important cube data
    static std::array<unsigned int, 6> FaceIndices;
    static std::array<Vector3f, 6> DirectionList;
    static std::map<unsigned int, Vector3f> FaceNormals;
    static std::map<unsigned int, std::array<Vector3f, 4>> FaceVertices;
    static std::vector<Vector2f> TextureCoordinatesList;

    static unsigned int ConvertDirectionToNumber(Vector3f Direction);
};