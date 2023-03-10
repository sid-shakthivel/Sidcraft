#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Block.h"

static const unsigned int CHUNK_SIZE = 16;
static const unsigned int CHUNK_HEIGHT = 48;
static const int WATER_LEVEL = 7;

// Represents a number of cubes together
class Chunk : public Mesh
{
private:
    bool IsWithinRange(Vector3f Vec);

    unsigned int WaterVAO, WaterVBO, WaterEBO;
    std::vector<Vertex> WaterVertices;
    std::vector<unsigned int> WaterIndices;

public:
    BlockType Blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];

    Chunk(Vector3f Offset, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE]);
    Chunk(const BlockType (&BlocksToCopy)[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE]);
    void CreateMesh();
    void Draw(Shader *MeshShader, Matrix4f Offset) const;
    void DrawWater(Shader *WaterShader, Matrix4f Offset) const;
    bool IsWithinChunk(Vector3f Vec, Matrix4f Offset) const;
    void SetChunk(Vector3f Position, Matrix4f Offset, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE]);
    void ClearChunk(Vector3f Position, Matrix4f Offset);
};