#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <glm/gtc/noise.hpp>

#include "../include/Matrix.h"
#include "../include/Shader.h"
#include "../include/Mesh.h"
#include "../include/Cube.h"
#include "../include/TextureAtlas.h"

#include "../include/Chunk.h"

std::vector<Vector3f> TestList = {UP, DOWN, LEFT, RIGHT, FRONT, BACK};

bool Chunk::IsWithinRange(Vector3f Vec)
{
    if (Vec.x < 0 || Vec.x >= CHUNK_SIZE || Vec.y < 0 || Vec.y >= CHUNK_HEIGHT || Vec.z < 0 || Vec.z >= CHUNK_SIZE)
        return true;
    return false;
}

/*
    Determines whether a block should be rendered or not
    In future when we can delete blocks may come in handy
*/
Chunk::Chunk(Vector3f Offset, int (&Heightmap)[160][160])
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
            height = (height + 1) / 4;

            height *= CHUNK_HEIGHT;

            for (int y = 0; y < height; y++)
                Blocks[x][y][z] = true;

            int ZOffset = (int)(abs(Offset.z) * CHUNK_SIZE) + z;
            int XOffset = (int)(abs(Offset.x) * CHUNK_SIZE) + x;

            Heightmap[ZOffset][XOffset] = height;
        }
}

void Chunk::CreateMesh()
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

                for (Vector3f Direction : TestList)
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

                            Faces.push_back(Direction);

                            indexer += 1;
                        }
                    }
                }
            }

    Mesh::CreateMesh();
}

void Chunk::Draw(Shader *MeshShader, bool isDepth, Matrix4f Offset) const
{
    // std::cout << Indices.size() << std::endl
    //           << Faces.size() << std::endl;

    // std::exit(0);

    MeshShader->SetMatrix4f("model", (const float *)(&Offset));

    glBindVertexArray(VAO);

    // Draw each face of the chunk

    for (int i = 0; i < Faces.size(); i++)
    {
        auto TempFace = Faces[i];
        if (!isDepth)
        {
            if (TempFace.IsEqual(TestList[0]) || TempFace.IsEqual(TestList[1]))
            {
                MeshShader->SetFloat("TestIndex", TextureAtlas::GetInstance()->FetchGrassTop());
            }
            else
            {
                MeshShader->SetFloat("TestIndex", TextureAtlas::GetInstance()->FetchGrassSide());
            }
        }

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)((i * 6) * sizeof(GLuint)));
    }
}