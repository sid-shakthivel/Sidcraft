#pragma once

#include "Matrix.h"
#include "Shader.h"
#include <vector>

const Vector3f UP = Vector3f(0.0f, 1.0f, 0.0f);
const Vector3f DOWN = Vector3f(0.0f, -1.0f, 0.0f);
const Vector3f LEFT = Vector3f(-1.0f, 0.0f, 0.0f);
const Vector3f RIGHT = Vector3f(1.0f, 0.0f, 0.0f);
const Vector3f FRONT = Vector3f(0.0f, 0.0f, 1.0f);
const Vector3f BACK = Vector3f(0.0f, 0.0f, -1.0f);

inline std::vector<Vector3f> DirectionsList = {UP, DOWN, LEFT, RIGHT, FRONT, BACK};

struct Vertex
{
    Vector3f Position;
    Vector3f Normal;
    Vector2f TextureCoordinates;

    Vertex(Vector3f Position, Vector3f Normal, Vector2f TextureCoordinates)
    {
        this->Position = Position;
        this->Normal = Normal;
        this->TextureCoordinates = TextureCoordinates;
    }
};

class Mesh
{
private:
    void InitaliseData();

protected:
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Vector3f> Faces;
    std::vector<Vector2f> TextureCoordinatesList = {Vector2f(0, 0), Vector2f(1, 0), Vector2f(1, 1), Vector2f(0, 1)};

    unsigned int VAO, VBO, EBO;

public:
    Mesh();
    void CreateMesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices);
    void CreateMesh();
    void Draw(Shader *MeshShader, bool isDepth);
};