#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <random>
#include <glm/gtc/noise.hpp>

#include "../include/Vector.h"
#include "../include/Matrix.h"
#include "../include/Shader.h"
#include "../include/Mesh.h"
#include "../include/Cube.h"
#include "../include/TextureAtlas.h"
#include "../include/World.h"
#include "../include/Camera.h"

#include "../include/Chunk.h"

/*
    Creates a new chunk by copying the blocks from another chunk
*/
Chunk::Chunk(const BlockType (&BlocksToCopy)[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE])
{
    std::copy(&BlocksToCopy[0][0][0], &BlocksToCopy[0][0][0] + (CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE), &Blocks[0][0][0]);
    // CreateMesh();
}

Chunk::Chunk(Vector3f Offset, int VAO, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE])
{
    // Lambda to determine type of block depending on height
    auto DetermineBlockType = [](float Y, float Height) -> BlockType
    {
        if (Y > (Height - 1))
            return BlockType::Grass;
        else if (Y > (Height - 3))
            return BlockType::Dirt;
        else
            return BlockType::Stone;
    };

    isDirty = true;

    // Initialise blocks to all be air blocks by default
    std::fill(&Blocks[0][0][0], &Blocks[0][0][0] + sizeof(Blocks) / sizeof(BlockType), BlockType::Air);

    const int WATER_SAND_OFFSET = 5;

    /*
        Uses perlin noise to determine height
        Build terrain by making each column a different / same height
    */
    for (int z = 0; z < CHUNK_SIZE; z++)
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            int VoxelX = x + (Offset.x * CHUNK_SIZE);
            int VoxelZ = z + (Offset.z * CHUNK_SIZE);

            int ZOffset = (int)(Offset.z * CHUNK_SIZE) + z;
            int XOffset = (int)(Offset.x * CHUNK_SIZE) + x;

            auto height = glm::simplex(glm::vec2(VoxelX / 64.0f, VoxelZ / 64.0f));

            height = (height + 1) / 4;
            height *= std::max(0.0f, 1.0f - GetGradient(XOffset, ZOffset));
            height *= CHUNK_HEIGHT * 1.5;

            // Handle general terrain
            for (int y = 0; y <= height; y++)
                Blocks[x][y][z] = DetermineBlockType(y, height);

            Heightmap[ZOffset][XOffset] = height;
        }

    /*
        Generate water by setting a limit for regions where water can be if it's currently air
        Generate sand which is generated around water
    */
    for (int z = 0; z < CHUNK_SIZE; z++)
        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int y = 0; y < WATER_LEVEL; y++)
                if (Blocks[x][y][z] == BlockType::Air)
                {
                    Blocks[x][y][z] = BlockType::Water;

                    int ZOffset = (int)(abs(Offset.z) * CHUNK_SIZE) + z;
                    int XOffset = (int)(abs(Offset.x) * CHUNK_SIZE) + x;

                    Heightmap[ZOffset][XOffset] = WATER_LEVEL;

                    for (int i = std::max(0, x - WATER_SAND_OFFSET); i < std::min((int)CHUNK_SIZE, x + WATER_SAND_OFFSET); i++)
                        for (int j = std::max(0, z - WATER_SAND_OFFSET); j < std::min((int)CHUNK_SIZE, z + WATER_SAND_OFFSET); j++)
                            for (int k = std::max(0, y - 2); k <= (y + WATER_SAND_OFFSET); k++)
                                if (Blocks[i][k][j] == BlockType::Grass || Blocks[i][k][j] == BlockType::Stone || Blocks[i][k][j] == BlockType::Dirt)
                                    Blocks[i][k][j] = BlockType::Sand;
                }

    // Generate trees

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> TreeRange(1, CHUNK_SIZE - 1);
    std::uniform_int_distribution<> TrunkRange(5, 7);

    for (int i = 0; i < 1; i++)
    {
        int TreeX = TreeRange(gen);
        int TreeZ = TreeRange(gen);

        int ZOffset = (int)(abs(Offset.z) * CHUNK_SIZE) + TreeZ;
        int XOffset = (int)(abs(Offset.x) * CHUNK_SIZE) + TreeX;

        int TreeY = Heightmap[ZOffset][XOffset];

        if (TreeY <= WATER_LEVEL)
            continue;

        auto TrunkHeight = TrunkRange(gen) + TreeY;

        for (int y = TreeY; y < TrunkHeight; y++)
            Blocks[TreeX][y][TreeZ] = BlockType::TreeTrunk;

        for (int x = std::max(0, TreeX - 1); x <= TreeX + 1; x++)
            for (int z = std::max(0, TreeZ - 1); z <= TreeZ + 1; z++)
                for (int y = TrunkHeight; y <= (TrunkHeight + 2); y++)
                    Blocks[x][y][z] = BlockType::TreeLeaves;
    }

    // Generate flowers

    for (int i = 0; i < 1; i++)
    {
        int FlowerX = TreeRange(gen);
        int FlowerZ = TreeRange(gen);

        int ZOffset = (int)(Offset.z * CHUNK_SIZE) + FlowerX;
        int XOffset = (int)(Offset.x * CHUNK_SIZE) + FlowerZ;

        int FlowerY = Heightmap[ZOffset][XOffset];

        if (FlowerY <= WATER_LEVEL)
            continue;

        // std::cout << "FlowerY: " << FlowerY << " FlowerZ: " << FlowerZ << " FlowerX: " << FlowerX << std::endl;

        // std::cout << "FlowerY: " << FlowerY << " FlowerZ: " << FlowerZ << " FlowerX: " << FlowerX << " It's " << Blocks[FlowerX][FlowerY][FlowerZ] << std::endl;

        // if (Blocks[FlowerX][FlowerY][FlowerZ] == BlockType::Air)
        Blocks[FlowerX][FlowerY][FlowerZ] = BlockType::Flower;
    }

    WaterMesh = new Mesh();
    TerrainMesh = new Mesh();

    Matrix4f OffsetMatrix = Matrix4f(1);
    OffsetMatrix.Translate(Vector3f(Offset.x * CHUNK_SIZE, 0, Offset.z * CHUNK_SIZE));

    WaterMesh->SetModel(OffsetMatrix);
    TerrainMesh->SetModel(OffsetMatrix);

    CreateMesh();

    TerrainMesh->Initialise();
    WaterMesh->Initialise();
}

Chunk::~Chunk()
{
    delete WaterMesh;
    delete TerrainMesh;
}

bool Chunk::IsWithinChunk(Vector3f Vec) const
{
    // Extract both the offset and calculate the relative postion
    // Vector3f ExtractedOffsetVec = this->OffsetMatrix.ExtractTranslation();
    // Vector3f RelativeVec = Vec.Sub(ExtractedOffsetVec);

    // Vec.x = round(Vec.x);
    // Vec.y = round(Vec.y);
    // Vec.z = round(Vec.z);

    // // Check whether the vector is within this specific chunk
    // if (Vec.y >= 0 && Vec.y <= CHUNK_HEIGHT)
    //     if (Vec.x >= ExtractedOffsetVec.x && Vec.x <= (ExtractedOffsetVec.x + CHUNK_SIZE))
    //         if (Vec.z >= ExtractedOffsetVec.z && Vec.z <= (ExtractedOffsetVec.z + CHUNK_SIZE))
    //             if (Blocks[(int)round(RelativeVec.x)][(int)round(RelativeVec.y)][(int)round(RelativeVec.z)] != BlockType::Air)
    //                 return true;

    return false;
}

void Chunk::SetChunk(Vector3f Position, Matrix4f Offset, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE])
{

    Vector3f RelativeVec = Position - Offset.ExtractTranslation();
    RelativeVec.RoundToNearestInt();

    // Blocks[(int)RelativeVec.x][(int)RelativeVec.y][(int)RelativeVec.z] = Camera::GetInstance()->GetSelectedBlockType();
}

float Chunk::GetGradient(float X, float Z)
{
    float Length = 17 * 17;

    float DistanceX = abs(X - Length * 0.5f);
    float DistanceZ = abs(Z - Length * 0.5f);
    float Distance = sqrt(DistanceX * DistanceX + DistanceZ * DistanceZ);

    float MaxWidth = Length * 0.5f - 15.0f;
    float Delta = Distance / MaxWidth;
    return Delta * Delta;
}

void Chunk::ClearChunk(Vector3f Position, Matrix4f Offset)
{
    Vector3f RelativeVec = Position - Offset.ExtractTranslation();
    RelativeVec.RoundToNearestInt();

    BlockType *CurrentBlock = &Blocks[(int)RelativeVec.x][(int)RelativeVec.y][(int)RelativeVec.z];
    World::GetInstance()->Inventory.insert_or_assign(*CurrentBlock, World::GetInstance()->Inventory[*CurrentBlock]++);

    Blocks[(int)RelativeVec.x][(int)RelativeVec.y][(int)RelativeVec.z] = BlockType::Air;
}

void Chunk::CreateMesh()
{
    auto IsWithinRange = [](Vector3f &Vec) -> bool
    {
        if (Vec.x < 0 || Vec.x >= CHUNK_SIZE || Vec.y < 0 || Vec.y >= CHUNK_HEIGHT || Vec.z < 0 || Vec.z >= CHUNK_SIZE)
            return true;
        return false;
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

                if (Blocks[x][y][z] == BlockType::Flower)
                {
                    float TextureIndex = TextureAtlas::GetInstance()->FetchTexture(Blocks[x][y][z]);

                    for (auto &FlowerVertices : Cube::DiagonalVertices)
                    {
                        auto FlowerNormals = TerrainMesh->CaculateNormals(FlowerVertices);

                        for (auto index : Cube::FaceIndices)
                            TerrainMesh->Indices->emplace_back(index + 4 * Indexer);

                        for (unsigned int i = 0; i < FlowerVertices.size(); i++)
                            TerrainMesh->Vertices->emplace_back(Vertex(Position + FlowerVertices[i], FlowerNormals[i], Cube::TextureCoordinatesList[i], TextureIndex));

                        Indexer += 1;
                    }
                }

                for (Vector3f Direction : Cube::DirectionList)
                {
                    Vector3f PositionToCheck = Position + Direction;

                    BlockType BlockAtPos = Blocks[(int)PositionToCheck.x][(int)PositionToCheck.y][(int)PositionToCheck.z];

                    if (IsWithinRange(PositionToCheck) || BlockAtPos == BlockType::Air || BlockAtPos == BlockType::Water || BlockAtPos == BlockType::Flower)
                    {
                        if (Blocks[x][y][z] == BlockType::Air || Blocks[x][y][z] == BlockType::Flower)
                            continue;

                        // Fix for Up/Down
                        if (Direction.IsEqual(UP) || Direction.IsEqual(DOWN))
                            PositionToCheck = Position;

                        auto Index = Cube::ConvertDirectionToNumber(Direction);
                        Vector3f Normal = Cube::FaceNormals[Index];
                        auto CubeFaceVertices = Cube::FaceVertices[Index];

                        // Determine block type
                        float TextureIndex = TextureAtlas::GetInstance()->FetchTexture(Blocks[x][y][z], Direction);

                        if (Blocks[x][y][z] == BlockType::Water)
                        {
                            if (y == WATER_LEVEL - 1)
                            {
                                // Sort out indices
                                for (auto index : Cube::FaceIndices)
                                    WaterMesh->Indices->emplace_back(index + 4 * WaterIndexer);

                                // Sort out vertices
                                for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
                                    WaterMesh->Vertices->emplace_back(Vertex(PositionToCheck + CubeFaceVertices[i], Normal, Cube::TextureCoordinatesList[i], TextureIndex));

                                WaterIndexer += 1;
                            }
                        }
                        else
                        {
                            // Sort out indices
                            for (auto index : Cube::FaceIndices)
                                TerrainMesh->Indices->emplace_back(index + 4 * Indexer);

                            // Sort out vertices
                            for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
                                TerrainMesh->Vertices->emplace_back(Vertex(PositionToCheck + CubeFaceVertices[i], Normal, Cube::TextureCoordinatesList[i], TextureIndex));

                            Indexer += 1;
                        }
                    }
                }
            }
}

void Chunk::Draw(Shader *MeshShader)
{
    if (isDirty)
    {
        TerrainMesh->Draw(MeshShader);
        // isDirty = false;
    }
}

void Chunk::DrawWater(Shader *WaterShader)
{
    if (isDirty)
    {
        WaterMesh->Draw(WaterShader);
        // isDirty = false;
    }
}