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
} __attribute__((packed));

class Mesh
{
protected:
    unsigned int VAO, VBO, EBO;
    Matrix4f ModelMatrix = Matrix4f(1);

    std::vector<Vector3f> CaculateNormals(std::vector<Vector3f> Vertices);

public:
    std::vector<Vertex> *Vertices;
    std::vector<unsigned int> *Indices;

    Mesh();

    virtual void Draw(Shader *MeshShader);
    void SetModel(Matrix4f Model);

    void Initialise();
    // void Initialise(unsigned int SuppliedVAO);
};