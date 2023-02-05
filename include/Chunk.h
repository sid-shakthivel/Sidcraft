#pragma once

static const unsigned int CHUNK_SIZE = 16;
static const unsigned int CHUNK_HEIGHT = 48;

#include "Mesh.h"
#include "Shader.h"

// Represents a number of cubes together
class Chunk : public Mesh
{
private:
    bool IsWithinRange(Vector3f Vec);

public:
    bool Blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    Chunk(Vector3f Offset, int (&Heightmap)[160][160]);
    Chunk(const bool (&BlocksToCopy)[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE]);
    void CreateMesh();
    void Draw(Shader *MeshShader, bool isDepth, Matrix4f Offset) const;
    bool IsWithinChunk(Vector3f Vec, Matrix4f Offset) const;
    void ClearChunk(Vector3f Position, Matrix4f Offset);
    // void ClearChunk(Vector3f Position) const;
};