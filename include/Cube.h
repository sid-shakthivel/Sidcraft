#include "Matrix.h"
#include "Mesh.h"

#include <array>
#include <map>

#pragma once

struct MeshData
{
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    MeshData(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices)
    {
        this->Vertices = Vertices;
        this->Indices = Indices;
    }
};

class Cube : public Mesh
{
public:
    Cube(float TextureIndex = 0);
    MeshData GetCubeData();
    void CreateMesh();
    void Draw(Shader *MeshShader, Matrix4f Offset) const;

    static std::array<unsigned int, 6> FaceIndices;
    static std::array<Vector3f, 6> DirectionList;
    static std::map<unsigned int, Vector3f> FaceNormals;
    static std::map<unsigned int, std::array<Vector3f, 4>> FaceVertices;

    static unsigned int ConvertDirectionToNumber(Vector3f Direction);
};