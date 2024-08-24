#pragma once

#include "./Vector.h"
#include "./Mesh.h"
#include "./Shader.h"
#include "./Block.h"

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
    float GetGradient(float X, float Y);

    BlockType Blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    Mesh *WaterMesh;
    Mesh *TerrainMesh;

public:
    bool isDirty;

    Chunk(Vector3f Offset, int VAO, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE]);
    Chunk(const BlockType (&BlocksToCopy)[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE]);
    ~Chunk();

    void CreateMesh();
    void Draw(Shader *MeshShader);
    void DrawWater(Shader *WaterShader);
    bool IsWithinChunk(Vector3f Vec) const;
    void SetChunk(Vector3f Position, Matrix4f Offset, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE]);
    void ClearChunk(Vector3f Position, Matrix4f Offset);
};