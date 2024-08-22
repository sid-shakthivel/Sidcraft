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

Cube::Cube(float TextureIndex, Vector3f TranslationVector) : Mesh()
{
    ModelMatrix.Translate(TranslationVector);

    // Create mesh
    unsigned int indexer = 0;
    for (Vector3f Direction : DirectionList)
    {
        auto Index = Cube::ConvertDirectionToNumber(Direction);

        Vector3f Normal = Cube::FaceNormals[Index];
        auto CubeFaceVertices = Cube::FaceVertices[Index];

        for (auto index : Cube::FaceIndices)
            Indices->push_back(index + 4 * indexer);

        for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
            Vertices->push_back(Vertex(CubeFaceVertices[i], Normal, TextureCoordinatesList[i], TextureIndex));

        indexer += 1;
    }

    Initialise();
}

std::array<Vector3f, 6> Cube::FaceNormals = {Vector3f(0, -1, 0), Vector3f(0, 1, 0), Vector3f(-1, 0, 0), Vector3f(1, 0, 0), Vector3f(0, 0, -1), Vector3f(0, 0, 1)};
std::array<unsigned int, 6> Cube::FaceIndices = {0, 1, 2, 2, 3, 0};
std::array<Vector3f, 6> Cube::DirectionList = {UP, DOWN, LEFT, RIGHT, FRONT, BACK};

// Top, Bottom, Right, Left, Front, Back
std::array<std::array<Vector3f, 4>, 6> Cube::FaceVertices = {{
    {Vector3f(0.0f, 1.0f, 1.0f), Vector3f(1.0f, 1.0f, 1.0f), Vector3f(1.0f, 1.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f)},
    {Vector3f(0.0f, 0.0f, 1.0f), Vector3f(1.0f, 0.0f, 1.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f)},
    {Vector3f(1.0f, 0.0f, 0.0f), Vector3f(1.0f, 0.0f, 1.0f), Vector3f(1.0f, 1.0f, 1.0f), Vector3f(1.0f, 1.0f, 0.0f)},
    {Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f)},
    {Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f)},
    {Vector3f(0.0f, 0.0f, 1.0f), Vector3f(1.0f, 0.0f, 1.0f), Vector3f(1.0f, 1.0f, 1.0f), Vector3f(0.0f, 1.0f, 1.0f)},
}};

std::vector<Vector2f>
    Cube::TextureCoordinatesList = {Vector2f(0.0f, 0.0f), Vector2f(1.0f, 0.0f), Vector2f(1.0f, 1.0f), Vector2f(0.0f, 1.0f)};

unsigned int Cube::ConvertDirectionToNumber(Vector3f Direction)
{
    for (unsigned int i = 0; i < DirectionList.size(); i++)
        if (DirectionList.at(i).IsEqual(Direction))
            return i;

    std::cout << "Error: Unknown Direction";
    return -1;
}