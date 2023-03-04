#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <tuple>
#include <vector>
#include <array>
#include <string>

#include "../include/Mesh.h"
#include "../include/Matrix.h"

#include "../include/Cube.h"

Cube::Cube(float TextureIndex, Vector3f TranslationVector)
{
    Position.Translate(TranslationVector);

    // Generates a cube
    unsigned int indexer = 0;
    for (Vector3f Direction : DirectionList)
    {
        auto Index = Cube::ConvertDirectionToNumber(Direction);

        Vector3f Normal = Cube::FaceNormals[Index];
        auto CubeFaceVertices = Cube::FaceVertices[Index];

        for (auto index : Cube::FaceIndices)
            Indices.push_back(index + 4 * indexer);

        for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
            Vertices.push_back(Vertex(CubeFaceVertices[i], Normal, TextureCoordinatesList[i], TextureIndex));

        indexer += 1;
    }

    CreateMesh();
}

void Cube::CreateMesh()
{
    // Setup general
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0); // Position

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, Normal)));
    glEnableVertexAttribArray(1); // Normals

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, TextureCoordinates)));
    glEnableVertexAttribArray(2); // Texture Coordinates

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, TextureIndex)));
    glEnableVertexAttribArray(3); // Texture Index
}

MeshData Cube::GetCubeData()
{
    return MeshData(Vertices, Indices);
}

void Cube::Draw(Shader *MeshShader, Matrix4f Offset) const
{
    MeshShader->SetMatrix4f("Model", (const float *)(&Offset));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)(0 * sizeof(GLuint)));
}

void Cube::Draw(Shader *MeshShader) const
{
    MeshShader->SetMatrix4f("Model", (const float *)(&Position));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)(0 * sizeof(GLuint)));
}

std::map<unsigned int, Vector3f> Cube::FaceNormals = {
    {0, Vector3f(0, -1, 0)},
    {1, Vector3f(0, 1, 0)},
    {2, Vector3f(-1, 0, 0)},
    {3, Vector3f(1, 0, 0)},
    {4, Vector3f(0, 0, -1)},
    {5, Vector3f(0, 0, 1)},
};

std::array<unsigned int, 6> Cube::FaceIndices = {0, 1, 2, 2, 3, 0};
std::array<Vector3f, 6> Cube::DirectionList = {UP, DOWN, LEFT, RIGHT, FRONT, BACK};

std::map<unsigned int, std::array<Vector3f, 4>> Cube::FaceVertices = {
    {0, {Vector3f(-0.5f, 0.5f, 0.5f), Vector3f(0.5f, 0.5f, 0.5f), Vector3f(0.5f, 0.5f, -0.5f), Vector3f(-0.5f, 0.5f, -0.5f)}},
    {1, {Vector3f(-0.5f, -0.5f, 0.5f), Vector3f(0.5f, -0.5f, 0.5f), Vector3f(0.5f, -0.5f, -0.5f), Vector3f(-0.5f, -0.5f, -0.5f)}},
    {2, {Vector3f(0.5f, -0.5f, -0.5f), Vector3f(0.5f, -0.5f, 0.5f), Vector3f(0.5f, 0.5f, 0.5f), Vector3f(0.5f, 0.5f, -0.5f)}},
    {3, {Vector3f(-0.5f, -0.5f, -0.5f), Vector3f(-0.5f, -0.5f, 0.5f), Vector3f(-0.5f, 0.5f, 0.5f), Vector3f(-0.5f, 0.5f, -0.5f)}},
    {4, {Vector3f(-0.5f, -0.5f, -0.5f), Vector3f(0.5f, -0.5f, -0.5f), Vector3f(0.5f, 0.5f, -0.5f), Vector3f(-0.5f, 0.5f, -0.5f)}},
    {5, {Vector3f(-0.5f, -0.5f, 0.5f), Vector3f(0.5f, -0.5f, 0.5f), Vector3f(0.5f, 0.5f, 0.5f), Vector3f(-0.5f, 0.5f, 0.5f)}},
};

unsigned int Cube::ConvertDirectionToNumber(Vector3f Direction)
{
    for (unsigned int i = 0; i < DirectionList.size(); i++)
        if (DirectionList.at(i).IsEqual(Direction))
            return i;

    std::cout << "oh no";
    return -1;
}