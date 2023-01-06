#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <tuple>
#include <vector>
#include <string>

#include "../include/Mesh.h"
#include "../include/Matrix.h"

#include "../include/Cube.h"

std::vector<Vector3f> AnotherList = {UP, DOWN, LEFT, RIGHT, FRONT, BACK};

Cube::Cube()
{
    // Generates a cube
    unsigned int indexer = 0;
    for (Vector3f Direction : AnotherList)
    {
        auto [CubeFaceVertices, CubeFaceIndices] = ::GetCubeData(Direction, Vector3f(0, 0, 0));

        std::vector<Vector3f> NormalsList;

        auto Tri1Corn1 = CubeFaceVertices[0];
        auto Tri1Corn2 = CubeFaceVertices[1];
        auto Tri1Corn3 = CubeFaceVertices[2];

        auto Tri2Corn1 = CubeFaceVertices[2];
        auto Tri2Corn2 = CubeFaceVertices[3];
        auto Tri2Corn3 = CubeFaceVertices[0];

        NormalsList.push_back(Tri1Corn1.CrossProduct(Tri1Corn2.Sub(Tri1Corn1), Tri1Corn3.Sub(Tri1Corn1)).ReturnNormalise());
        NormalsList.push_back(Tri1Corn1.CrossProduct(Tri1Corn2.Sub(Tri1Corn1), Tri1Corn3.Sub(Tri1Corn1)).ReturnNormalise());
        NormalsList.push_back(Tri1Corn1.CrossProduct(Tri1Corn2.Sub(Tri1Corn1), Tri1Corn3.Sub(Tri1Corn1)).ReturnNormalise());

        NormalsList.push_back(Tri2Corn1.CrossProduct(Tri2Corn2.Sub(Tri2Corn1), Tri2Corn3.Sub(Tri2Corn1)).ReturnNormalise());
        NormalsList.push_back(Tri2Corn1.CrossProduct(Tri2Corn2.Sub(Tri2Corn1), Tri2Corn3.Sub(Tri2Corn1)).ReturnNormalise());
        NormalsList.push_back(Tri2Corn1.CrossProduct(Tri2Corn2.Sub(Tri2Corn1), Tri2Corn3.Sub(Tri2Corn1)).ReturnNormalise());

        for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
            Vertices.push_back(Vertex(CubeFaceVertices[i], NormalsList[i], TextureCoordinatesList[i]));

        std::for_each(CubeFaceIndices.begin(), CubeFaceIndices.end(), [indexer](unsigned int &index)
                      { index += 4 * indexer; });

        for (auto index : CubeFaceIndices)
            Indices.push_back(index);

        Faces.push_back(Direction);

        indexer += 1;
    }
}

MeshData Cube::GetCubeData()
{
    return MeshData(Vertices, Indices, Faces);
}

std::tuple<std::vector<Vector3f>, std::vector<unsigned int>> GetCubeData(Vector3f Direction, Vector3f Position)
{
    std::vector<Vector3f> FaceVertices;
    std::vector<unsigned int> FaceIndices;

    if (Direction.IsEqual(UP))
    {
        FaceVertices.push_back(Vector3f(-0.5f, 0.5f, 0.5f));  // 1
        FaceVertices.push_back(Vector3f(0.5f, 0.5f, 0.5f));   // 2
        FaceVertices.push_back(Vector3f(0.5f, 0.5f, -0.5f));  // 3
        FaceVertices.push_back(Vector3f(-0.5f, 0.5f, -0.5f)); // 4

        FaceIndices = {0, 1, 2, 2, 3, 0};
    }
    else if (Direction.IsEqual(DOWN))
    {
        FaceVertices.push_back(Vector3f(-0.5f, -0.5f, 0.5f));
        FaceVertices.push_back(Vector3f(0.5f, -0.5f, 0.5f));
        FaceVertices.push_back(Vector3f(0.5f, -0.5f, -0.5f));
        FaceVertices.push_back(Vector3f(-0.5f, -0.5f, -0.5f));

        FaceIndices = {0, 1, 2, 2, 3, 0};
    }
    else if (Direction.IsEqual(LEFT))
    {
        FaceVertices.push_back(Vector3f(0.5f, -0.5f, -0.5f));
        FaceVertices.push_back(Vector3f(0.5f, -0.5f, 0.5f));
        FaceVertices.push_back(Vector3f(0.5f, 0.5f, 0.5f));
        FaceVertices.push_back(Vector3f(0.5f, 0.5f, -0.5f));
        FaceIndices = {0, 1, 2, 2, 3, 0};
    }
    else if (Direction.IsEqual(RIGHT))
    {
        FaceVertices.push_back(Vector3f(-0.5f, -0.5f, -0.5f));
        FaceVertices.push_back(Vector3f(-0.5f, -0.5f, 0.5f));
        FaceVertices.push_back(Vector3f(-0.5f, 0.5f, 0.5f));
        FaceVertices.push_back(Vector3f(-0.5f, 0.5f, -0.5f));

        // FaceIndices = {0, 1, 2, 2, 3, 1};

        FaceIndices = {0, 1, 2, 2, 3, 0};
    }
    else if (Direction.IsEqual(FRONT))
    {
        FaceVertices.push_back(Vector3f(-0.5f, -0.5f, -0.5f));
        FaceVertices.push_back(Vector3f(0.5f, -0.5f, -0.5f));
        FaceVertices.push_back(Vector3f(0.5f, 0.5f, -0.5f));
        FaceVertices.push_back(Vector3f(-0.5f, 0.5f, -0.5f));

        FaceIndices = {0, 1, 2, 2, 3, 0};
    }
    else
    {
        // Back
        FaceVertices.push_back(Vector3f(-0.5f, -0.5f, 0.5f));
        FaceVertices.push_back(Vector3f(0.5f, -0.5f, 0.5f));
        FaceVertices.push_back(Vector3f(0.5f, 0.5f, 0.5f));
        FaceVertices.push_back(Vector3f(-0.5f, 0.5f, 0.5f));

        FaceIndices = {0, 1, 2, 2, 3, 0};
    }

    // Vertex positions must be offsetted by the position
    std::for_each(FaceVertices.begin(), FaceVertices.end(), [Position](auto &vertex)
                  { vertex = vertex.Add(Position); });

    return {FaceVertices, FaceIndices};
}