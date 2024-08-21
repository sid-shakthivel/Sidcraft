#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Block.h"

static const unsigned int CHUNK_SIZE = 16;
static const unsigned int CHUNK_HEIGHT = 48;
static const int WATER_LEVEL = 7;

/*
    Chunks are sections of the game world.
    16*16*48 voxels
    Optimises rendering and memory
*/
class Chunk
{
private:
    bool IsWithinRange(Vector3f Vec);

    Mesh *WaterMesh;
    Mesh *TerrainMesh;

public:
    BlockType Blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];

    Chunk(Vector3f Offset, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE]);
    Chunk(const BlockType (&BlocksToCopy)[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE]);

    void CreateMesh();
    void Draw(Shader *MeshShader);
    void DrawWater(Shader *WaterShader) const;
    bool IsWithinChunk(Vector3f Vec) const;
    void SetChunk(Vector3f Position, Matrix4f Offset, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE]);
    void ClearChunk(Vector3f Position, Matrix4f Offset);
};