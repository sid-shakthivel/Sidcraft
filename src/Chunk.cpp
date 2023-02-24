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
#include "../include/World.h"
#include "../include/Camera.h"

#include "../include/Chunk.h"

std::vector<Vector3f> TestList = {UP, DOWN, LEFT, RIGHT, FRONT, BACK};

bool Chunk::IsWithinRange(Vector3f Vec)
{
    if (Vec.x < 0 || Vec.x >= CHUNK_SIZE || Vec.y < 0 || Vec.y >= CHUNK_HEIGHT || Vec.z < 0 || Vec.z >= CHUNK_SIZE)
        return true;
    return false;
}

bool Chunk::IsWithinChunk(Vector3f Vec, Matrix4f Offset) const
{
    // Extract both the offset and calculate the relative postion
    Vector3f ExtractedOffsetVec = Offset.ExtractTranslation();
    Vector3f RelativeVec = Vec.Sub(ExtractedOffsetVec);

    Vec.x = round(Vec.x);
    Vec.y = round(Vec.y);
    Vec.z = round(Vec.z);

    // Check whether the vector is within this specific chunk
    if (Vec.y >= 0 && Vec.y <= CHUNK_HEIGHT)
        if (Vec.x >= ExtractedOffsetVec.x && Vec.x <= (ExtractedOffsetVec.x + CHUNK_SIZE))
            if (Vec.z >= ExtractedOffsetVec.z && Vec.z <= (ExtractedOffsetVec.z + CHUNK_SIZE))
                if (Blocks[(int)round(RelativeVec.x)][(int)round(RelativeVec.y)][(int)round(RelativeVec.z)] != BlockType::Air)
                    return true;

    return false;
}

void Chunk::SetChunk(Vector3f Position, Matrix4f Offset, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE])
{

    Vector3f RelativeVec = Position.Sub(Offset.ExtractTranslation());
    RelativeVec.RoundToNearestInt();

    for (int i = 0; i < 10; i++)
        Blocks[(int)RelativeVec.x][(int)RelativeVec.y + i][(int)RelativeVec.z] = Camera::GetInstance()->GetSelectedBlockType();
}

void Chunk::ClearChunk(Vector3f Position, Matrix4f Offset)
{
    Vector3f RelativeVec = Position.Sub(Offset.ExtractTranslation());
    RelativeVec.RoundToNearestInt();

    BlockType *CurrentBlock = &Blocks[(int)RelativeVec.x][(int)RelativeVec.y][(int)RelativeVec.z];
    World::GetInstance()->Inventory.insert_or_assign(*CurrentBlock, World::GetInstance()->Inventory[*CurrentBlock]++);

    Blocks[(int)RelativeVec.x][(int)RelativeVec.y][(int)RelativeVec.z] = BlockType::Air;
}

// Creates a new chunk
Chunk::Chunk(const BlockType (&BlocksToCopy)[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE])
{
    for (int x = 0; x < CHUNK_SIZE; x++)
        for (int z = 0; z < CHUNK_SIZE; z++)
            for (int y = 0; y < CHUNK_HEIGHT; y++)
                Blocks[x][y][z] = BlocksToCopy[x][y][z];
}

float GetGradient(float X, float Z)
{
    float Length = 17 * 17;

    float DistanceX = abs(X - Length * 0.5f);
    float DistanceZ = abs(Z - Length * 0.5f);
    float Distance = sqrt(DistanceX * DistanceX + DistanceZ * DistanceZ);

    float MaxWidth = Length * 0.5f - 15.0f;
    float Delta = Distance / MaxWidth;
    return Delta * Delta;
}

/*
    Determines whether a block should be rendered or not
    In future when we can delete blocks may come in handy
*/
Chunk::Chunk(Vector3f Offset, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE])
{
    for (int x = 0; x < CHUNK_SIZE; x++)
        for (int z = 0; z < CHUNK_SIZE; z++)
            for (int y = 0; y < CHUNK_HEIGHT; y++)
                Blocks[x][y][z] = BlockType::Air;

    auto DetermineBlockType = [](float Y, float Height) -> BlockType
    {
        if (Y > (Height - 1))
            return BlockType::Grass;
        else
            return BlockType::Stone;
        // else if (Y > (Height - 3))
        //     return BlockType::Dirt;
        // else
        //     return BlockType::Stone;
    };

    /*
        Uses perlin noise to determine height
        Build terrain by making each column a different / same height
    */
    for (int z = 0; z < CHUNK_SIZE; z++)
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            int VoxelX = x + (Offset.x * CHUNK_SIZE);
            int VoxelZ = z + (Offset.z * CHUNK_SIZE);

            int ZOffset = (int)(abs(Offset.z) * CHUNK_SIZE) + z;
            int XOffset = (int)(abs(Offset.x) * CHUNK_SIZE) + x;

            auto height = glm::simplex(glm::vec2(VoxelX / 64.0f, VoxelZ / 64.0f));

            height = (height + 1) / 4;

            height *= std::max(0.0f, 1.0f - GetGradient(XOffset, ZOffset));

            height *= CHUNK_HEIGHT * 1.5;

            for (int y = 0; y <= height; y++)
                Blocks[x][y][z] = DetermineBlockType(y, height);

            Heightmap[ZOffset][XOffset] = height;
        }

    const int OFFSET = 5;
    // Generate water by setting a limit for regions where water can be if it's currently air
    // Generate sand which is generated around water
    for (int z = 0; z < CHUNK_SIZE; z++)
        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int y = 0; y < WATER_LEVEL; y++)
                if (Blocks[x][y][z] == BlockType::Air)
                {
                    Blocks[x][y][z] = BlockType::Water;

                    int ZOffset = (int)(abs(Offset.z) * CHUNK_SIZE) + z;
                    int XOffset = (int)(abs(Offset.x) * CHUNK_SIZE) + x;

                    Heightmap[ZOffset][XOffset] = WATER_LEVEL;

                    Blocks[x][0][z] = BlockType::Sand;

                    // for (int i = std::max(0, x - OFFSET); i < std::min((int)CHUNK_SIZE, x + OFFSET); i++)
                    //     for (int j = std::max(0, z - OFFSET); j < std::min((int)CHUNK_SIZE, z + OFFSET); j++)
                    //         for (int k = y - 2; k <= (y + OFFSET); k++)
                    //             if (Blocks[i][k][j] == BlockType::Grass || Blocks[i][k][j] == BlockType::Stone || Blocks[i][k][j] == BlockType::Dirt)
                    //                 Blocks[i][k][j] = BlockType::Sand;
                }
}

void Chunk::CreateMesh()
{
    auto GetTextureIndex = [](BlockType TypeBlock, Vector3f Direction) -> float
    {
        switch (TypeBlock)
        {
        case BlockType::Grass:
            return Direction.IsEqual(TestList[0]) || Direction.IsEqual(TestList[1]) ? TextureAtlas::GetInstance()->FetchGrassTop() : TextureAtlas::GetInstance()->FetchGrassSide();
        case BlockType::Dirt:
            return TextureAtlas::GetInstance()->FetchDirt();
        case BlockType::Stone:
            return TextureAtlas::GetInstance()->FetchStone();
        case BlockType::Water:
            return TextureAtlas::GetInstance()->FetchWater();
        case BlockType::Sand:
            return TextureAtlas::GetInstance()->FetchSand();
        default:
            std::cout << "ERROR: Unknown Block Type" << std::endl;
            std::exit(0);
        }
    };

    /*
            Loop through each block, and check if adjacent blocks are within the chunk
            If an adjacent block is within the chunk, it need not be rendered
            Else the adjacent block is an air block and thus this face might be visible so can be rendered
        */
    unsigned int Indexer = 0;
    unsigned int WaterIndexer = 0;
    for (int x = 0; x < CHUNK_SIZE; x++)
        for (int y = 0; y < CHUNK_HEIGHT; y++)
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                Vector3f Position = Vector3f(x, y, z);

                for (Vector3f Direction : Cube::DirectionList)
                {
                    Vector3f PositionToCheck = Position.Add(Direction);

                    if (IsWithinRange(PositionToCheck) || Blocks[(int)PositionToCheck.x][(int)PositionToCheck.y][(int)PositionToCheck.z] == BlockType::Air || Blocks[(int)PositionToCheck.x][(int)PositionToCheck.y][(int)PositionToCheck.z] == BlockType::Water)
                    {
                        if (Blocks[x][y][z] != BlockType::Air)
                        {
                            // Fix for Up/Down
                            if (Direction.IsEqual(UP) || Direction.IsEqual(DOWN))
                                PositionToCheck = Position;

                            auto Index = Cube::ConvertDirectionToNumber(Direction);
                            Vector3f Normal = Cube::FaceNormals[Index];
                            auto CubeFaceVertices = Cube::FaceVertices[Index];

                            // Determine block type
                            float TextureIndex = GetTextureIndex(Blocks[x][y][z], Direction);

                            if (Blocks[x][y][z] != BlockType::Water)
                            {
                                // Sort out indices
                                for (auto index : Cube::FaceIndices)
                                    Indices.push_back(index + 4 * Indexer);

                                // Sort out vertices
                                for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
                                    Vertices.push_back(Vertex(PositionToCheck.Add(CubeFaceVertices[i]), Normal, TextureCoordinatesList[i], TextureIndex));

                                Indexer += 1;
                            }
                        }
                    }
                }
            }

    /*
        Water is separated from the other materials
        Water has special properties and has a specific shader
    */
    for (int x = 0; x < CHUNK_SIZE; x++)
        for (int y = 0; y <= WATER_LEVEL; y++)
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                Vector3f Position = Vector3f(x, y, z);

                for (Vector3f Direction : Cube::DirectionList)
                {
                    Vector3f PositionToCheck = Position.Add(Direction);

                    if (IsWithinRange(PositionToCheck) || Blocks[(int)PositionToCheck.x][(int)PositionToCheck.y][(int)PositionToCheck.z] == false)
                    {
                        if (Blocks[x][y][z] != BlockType::Air)
                        {
                            // Fix for Up/Down
                            if (Direction.IsEqual(UP) || Direction.IsEqual(DOWN))
                                PositionToCheck = Position;

                            auto Index = Cube::ConvertDirectionToNumber(Direction);
                            Vector3f Normal = Cube::FaceNormals[Index];
                            auto CubeFaceVertices = Cube::FaceVertices[Index];

                            // Determine block type
                            float TextureIndex = GetTextureIndex(Blocks[x][y][z], Direction);

                            /*
                                Water is separated from the other materials
                                Water has special properties and has a specific shader
                            */
                            if (Blocks[x][y][z] == BlockType::Water)
                            {
                                // Sort out indices
                                for (auto index : Cube::FaceIndices)
                                    WaterIndices.push_back(index + 4 * WaterIndexer);

                                // Sort out vertices
                                for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
                                    WaterVertices.push_back(Vertex(PositionToCheck.Add(CubeFaceVertices[i]), Normal, TextureCoordinatesList[i], TextureIndex));

                                WaterIndexer += 1;
                            }
                        }
                    }
                }
            }

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

    // // Setup water
    glGenVertexArrays(1, &WaterVAO);
    glBindVertexArray(WaterVAO);

    glGenBuffers(1, &WaterVBO);
    glBindBuffer(GL_ARRAY_BUFFER, WaterVBO);
    glBufferData(GL_ARRAY_BUFFER, WaterVertices.size() * sizeof(Vertex), &WaterVertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &WaterEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, WaterEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, WaterIndices.size() * sizeof(unsigned int), &WaterIndices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0); // Position

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, Normal)));
    glEnableVertexAttribArray(1); // Normals

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, TextureCoordinates)));
    glEnableVertexAttribArray(2); // Texture Coordinates

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, TextureIndex)));
    glEnableVertexAttribArray(3); // Texture Index
}

void Chunk::Draw(Shader *MeshShader, bool isDepth, Matrix4f Offset) const
{
    MeshShader->SetMatrix4f("Model", (const float *)(&Offset));

    if (!isDepth)
        MeshShader->SetFloat("Time", 1.0f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, (void *)(0 * sizeof(GLuint)));
}

void Chunk::DrawWater(Shader *WaterShader, Matrix4f Offset) const
{
    WaterShader->SetMatrix4f("Model", (const float *)(&Offset));

    glBindVertexArray(WaterVAO);
    glDrawElements(GL_TRIANGLES, WaterIndices.size(), GL_UNSIGNED_INT, (void *)(0 * sizeof(GLuint)));
}