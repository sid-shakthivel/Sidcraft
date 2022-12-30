#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <tuple>
#include <vector>

#include "matrix.h"

const Vector3f UP = Vector3f(0.0f, 1.0f, 0.0f);
const Vector3f DOWN = Vector3f(0.0f, -1.0f, 0.0f);
const Vector3f LEFT = Vector3f(-1.0f, 0.0f, 0.0f);
const Vector3f RIGHT = Vector3f(1.0f, 0.0f, 0.0f);
const Vector3f FRONT = Vector3f(0.0f, 0.0f, 1.0f);
const Vector3f BACK = Vector3f(0.0f, 0.0f, -1.0f);

// Represents a number of blocks together
class Chunk
{
private:
    static const unsigned int CHUNK_SIZE = 16;
    bool Blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    std::vector<Vector3f> Vertices;
    std::vector<unsigned int> Indices;

    std::vector<Vector3f> DirectionsList = {UP, DOWN, LEFT, RIGHT, FRONT, BACK};

    unsigned int VAO, VBO, EBO;

    std::tuple<std::vector<Vector3f>, std::vector<unsigned int>> GetCubeData(Vector3f Direction, Vector3f Position)
    {
        std::vector<Vector3f> FaceVertices;
        std::vector<unsigned int> FaceIndices;

        if (Direction.IsEqual(UP))
        {
            FaceVertices.push_back(Vector3f(-0.5f, 0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, 0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, 0.5f, -0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, 0.5f, -0.5f));

            FaceIndices = {0, 1, 2, 2, 3, 0};
        }
        else if (Direction.IsEqual(DOWN))
        {
            FaceVertices.push_back(Vector3f(-0.5f, -0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, -0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, -0.5f, -0.5f));
            FaceVertices.push_back(Vector3f(0.5f, -0.5f, -0.5f));

            FaceIndices = {0, 1, 2, 2, 3, 1};
        }
        else if (Direction.IsEqual(LEFT))
        {
            FaceVertices.push_back(Vector3f(0.5f, 0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, -0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, 0.5f, -0.5f));
            FaceVertices.push_back(Vector3f(0.5f, -0.5f, -0.5f));

            FaceIndices = {0, 1, 2, 2, 3, 1};
        }
        else if (Direction.IsEqual(RIGHT))
        {
            FaceVertices.push_back(Vector3f(-0.5f, 0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, -0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, 0.5f, -0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, -0.5f, -0.5f));

            FaceIndices = {0, 1, 2, 2, 3, 1};
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

public:
    /*
        Determines whether a block should be rendered or not
        In future when we can delete blocks may come in handy
    */
    Chunk()
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int y = 0; y < CHUNK_SIZE; y++)
                for (int z = 0; z < CHUNK_SIZE; z++)
                    Blocks[x][y][z] = true;
    }

    void CreateMesh()
    {
        /*
            Loop through each block, and check if adjacent blocks are within the chunk
            If an adjacent block is within the chunk, it need not be rendered
            Else the adjacent block is an air block and thus this face might be visible so can be rendered
        */
        unsigned int indexer = 0;
        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int y = 0; y < CHUNK_SIZE; y++)
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    Vector3f Position = Vector3f(x, y, z);

                    for (Vector3f Direction : DirectionsList)
                    {
                        Vector3f PositionToCheck = Position.Add(Direction);

                        if (PositionToCheck.x < 0 || PositionToCheck.x >= CHUNK_SIZE || PositionToCheck.y < 0 || PositionToCheck.y >= CHUNK_SIZE || PositionToCheck.z < 0 || PositionToCheck.z >= CHUNK_SIZE)
                        {
                            // Fix for UP/Down
                            if (Direction.IsEqual(UP) || Direction.IsEqual(DOWN))
                                PositionToCheck = Position;

                            auto [CubeFaceVertices, CubeFaceIndices] = GetCubeData(Direction, PositionToCheck);
                            std::for_each(CubeFaceIndices.begin(), CubeFaceIndices.end(), [indexer](unsigned int &index)
                                          { index += 4 * indexer; });

                            Vertices.insert(Vertices.end(), CubeFaceVertices.begin(), CubeFaceVertices.end());
                            for (auto index : CubeFaceIndices)
                                Indices.push_back(index);

                            indexer += 1;
                        }
                    }
                }

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vector3f), &Vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // Setup position attribute
        glEnableVertexAttribArray(0);
    }

    void Draw()
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
    }
};
