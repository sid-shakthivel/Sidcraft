#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "Shader.h"

const Vector3f UP = Vector3f(0.0f, 1.0f, 0.0f);
const Vector3f DOWN = Vector3f(0.0f, -1.0f, 0.0f);
const Vector3f LEFT = Vector3f(-1.0f, 0.0f, 0.0f);
const Vector3f RIGHT = Vector3f(1.0f, 0.0f, 0.0f);
const Vector3f FRONT = Vector3f(0.0f, 0.0f, 1.0f);
const Vector3f BACK = Vector3f(0.0f, 0.0f, -1.0f);

struct Vertex
{
    uint32_t CondensedOther; // Contains Normal, TextureCoords, TextureIndex
    uint32_t CondensedPos;

    Vertex(Vector3f Position, Vector3f Normal, Vector2f TextureCoordinates, float TextureIndex);
    void Testing();
    Vector2f GetTextureCoordinates();
    Vector3f GetNormal();
} __attribute__((packed));

class Mesh
{
protected:
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Vector2f> TextureCoordinatesList = {Vector2f(0, 0), Vector2f(1, 0), Vector2f(1, 1), Vector2f(0, 1)};

    unsigned int VAO, VBO, EBO;

    std::vector<Vector3f> CaculateNormals(std::vector<Vector3f> Vertices)
    {
        std::vector<Vector3f> NormalsList;

        auto Tri1Corn1 = Vertices[0];
        auto Tri1Corn2 = Vertices[1];
        auto Tri1Corn3 = Vertices[2];

        auto Tri2Corn1 = Vertices[2];
        auto Tri2Corn2 = Vertices[3];
        auto Tri2Corn3 = Vertices[0];

        NormalsList.push_back(Tri1Corn1.CrossProduct(Tri1Corn2.Sub(Tri1Corn1), Tri1Corn3.Sub(Tri1Corn1)).ReturnNormalise());
        NormalsList.push_back(Tri1Corn1.CrossProduct(Tri1Corn2.Sub(Tri1Corn1), Tri1Corn3.Sub(Tri1Corn1)).ReturnNormalise());
        NormalsList.push_back(Tri1Corn1.CrossProduct(Tri1Corn2.Sub(Tri1Corn1), Tri1Corn3.Sub(Tri1Corn1)).ReturnNormalise());

        NormalsList.push_back(Tri2Corn1.CrossProduct(Tri2Corn2.Sub(Tri2Corn1), Tri2Corn3.Sub(Tri2Corn1)).ReturnNormalise());
        NormalsList.push_back(Tri2Corn1.CrossProduct(Tri2Corn2.Sub(Tri2Corn1), Tri2Corn3.Sub(Tri2Corn1)).ReturnNormalise());
        NormalsList.push_back(Tri2Corn1.CrossProduct(Tri2Corn2.Sub(Tri2Corn1), Tri2Corn3.Sub(Tri2Corn1)).ReturnNormalise());

        return NormalsList;
    }

public:
    Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices);
    Mesh();
    void Draw(Shader *MeshShader, bool isDepth);
    void CreateMesh();
    void InitaliseData(unsigned int *TempVAO, unsigned int *TempVBO, unsigned int *TempEBO, std::vector<Vertex> *TempVertices, std::vector<unsigned int> *TempIndices);
    unsigned int GetVAO() const;
};