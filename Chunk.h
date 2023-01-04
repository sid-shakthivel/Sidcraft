#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <tuple>
#include <vector>
#include <string>

#include "matrix.h"
#include "shader.h"
#include "mesh.h"
#include "TextureAtlas.h"
#include "PerlinNoise.hpp"

#include <glm/gtc/noise.hpp>

struct ThinVertex
{
    Vector3f Position;
    Vector2f TextureCoordinates;

    ThinVertex(Vector3f Position, Vector2f TextureCoordinates)
    {
        this->Position = Position;
        this->TextureCoordinates = TextureCoordinates;
    }
};

const Vector3f UP = Vector3f(0.0f, 1.0f, 0.0f);
const Vector3f DOWN = Vector3f(0.0f, -1.0f, 0.0f);
const Vector3f LEFT = Vector3f(-1.0f, 0.0f, 0.0f);
const Vector3f RIGHT = Vector3f(1.0f, 0.0f, 0.0f);
const Vector3f FRONT = Vector3f(0.0f, 0.0f, 1.0f);
const Vector3f BACK = Vector3f(0.0f, 0.0f, -1.0f);

static const unsigned int CHUNK_SIZE = 16;
static const unsigned int CHUNK_HEIGHT = 48;

std::vector<Vector3f> DirectionsList = {UP, DOWN, LEFT, RIGHT, FRONT, BACK};

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

// Represents a number of blocks together
class Chunk
{
private:
    bool Blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];

    std::vector<Vertex> VertexData;
    std::vector<unsigned int> Indices;
    std::vector<Vector3f> FaceList;

    std::vector<Vector2f> TextureCoordinatesList = {Vector2f(0, 0), Vector2f(1, 0), Vector2f(1, 1), Vector2f(0, 1)};

    unsigned int VAO, VBO, EBO;

    bool IsWithinRange(Vector3f Vec)
    {
        if (Vec.x < 0 || Vec.x >= CHUNK_SIZE || Vec.y < 0 || Vec.y >= CHUNK_HEIGHT || Vec.z < 0 || Vec.z >= CHUNK_SIZE)
            return true;
        return false;
    }

public:
    /*
        Determines whether a block should be rendered or not
        In future when we can delete blocks may come in handy
    */
    Chunk(Vector3f Offset, int *Heightmap)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int z = 0; z < CHUNK_SIZE; z++)
                for (int y = 0; y < CHUNK_HEIGHT; y++)
                    Blocks[x][y][z] = false;

        /*
            Uses perlin noise to determine height
            Build terrain by making each column a different / same height
        */
        for (int z = 0; z < CHUNK_SIZE; z++)
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                int VoxelX = x + (Offset.x * CHUNK_SIZE);
                int VoxelZ = z + (Offset.z * CHUNK_SIZE);

                auto height = glm::simplex(glm::vec2(VoxelX / 64.0f, VoxelZ / 64.0f));
                height = (height + 1) / 3;

                height *= CHUNK_HEIGHT;

                for (int y = 0; y < height; y++)
                    Blocks[x][y][z] = true;

                Heightmap[(int)(abs(Offset.z) * CHUNK_SIZE + abs(Offset.x)) + (int)(z * CHUNK_SIZE + x)] = height;
            }
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
            for (int y = 0; y < CHUNK_HEIGHT; y++)
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    Vector3f Position = Vector3f(x, y, z);

                    for (Vector3f Direction : DirectionsList)
                    {
                        Vector3f PositionToCheck = Position.Add(Direction);

                        if (IsWithinRange(PositionToCheck) || Blocks[(int)PositionToCheck.x][(int)PositionToCheck.y][(int)PositionToCheck.z] == false)
                        {
                            if (Blocks[x][y][z] == true)
                            {
                                // Fix for Up/Down
                                if (Direction.IsEqual(UP) || Direction.IsEqual(DOWN))
                                    PositionToCheck = Position;

                                auto [CubeFaceVertices, CubeFaceIndices] = GetCubeData(Direction, PositionToCheck);

                                std::for_each(CubeFaceIndices.begin(), CubeFaceIndices.end(), [indexer](unsigned int &index)
                                              { index += 4 * indexer; });

                                for (auto index : CubeFaceIndices)
                                    Indices.push_back(index);

                                std::vector<Vector3f> NormalsList;

                                auto Corner1 = CubeFaceVertices[0];
                                auto Corner2 = CubeFaceVertices[1];
                                auto Corner3 = CubeFaceVertices[2];
                                auto Corner4 = CubeFaceVertices[3];

                                NormalsList.push_back(Corner1.CrossProduct(Corner2, Corner4));
                                NormalsList.push_back(Corner1.CrossProduct(Corner1, Corner3));
                                NormalsList.push_back(Corner1.CrossProduct(Corner2, Corner4));
                                NormalsList.push_back(Corner1.CrossProduct(Corner1, Corner3));

                                for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
                                    VertexData.push_back(Vertex(CubeFaceVertices[i], NormalsList[i], TextureCoordinatesList[i]));

                                FaceList.push_back(Direction);

                                indexer += 1;
                            }
                        }
                    }
                }

        // Setup rendering buffers
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, VertexData.size() * sizeof(Vertex), &VertexData[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0); // Vertices
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float))); // Normals
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(6 * sizeof(float))); // TextureCoordinates
        glEnableVertexAttribArray(2);
    }

    void Draw(Shader *MeshShader)
    {
        MeshShader->SetFloat("TestIndex", TextureAtlas::GetInstance()->FetchGrassTop());
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, (void *)(0 * sizeof(GLuint)));
    }
};