#include "Matrix.h"
#include "Mesh.h"

#pragma once

struct MeshData
{
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Vector3f> Faces;

    MeshData(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices, std::vector<Vector3f> Faces)
    {
        this->Vertices = Vertices;
        this->Indices = Indices;
        this->Faces = Faces;
    }
};

class Cube : public Mesh
{
public:
    Cube(float TextureIndex = 0);
    MeshData GetCubeData();
    void CreateMesh();
    void Draw(Shader *MeshShader, Matrix4f Offset) const;
};

std::tuple<std::vector<Vector3f>, std::vector<unsigned int>> GetCubeData(Vector3f Direction, Vector3f Position);