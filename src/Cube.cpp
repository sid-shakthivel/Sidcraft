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

Cube::Cube(float TextureIndex)
{
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

        Faces.push_back(Direction);

        indexer += 1;
    }
}

void Cube::CreateMesh()
{
    Mesh::CreateMesh();
}

MeshData Cube::GetCubeData()
{
    return MeshData(Vertices, Indices, Faces);
}

void Cube::Draw(Shader *MeshShader, Matrix4f Offset) const
{
    MeshShader->SetMatrix4f("model", (const float *)(&Offset));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, (void *)(0 * sizeof(GLuint)));
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