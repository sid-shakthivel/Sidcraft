#pragma once

static const unsigned int CHUNK_SIZE = 16;
static const unsigned int CHUNK_HEIGHT = 48;

#include "Mesh.h"
#include "Shader.h"

// Represents a number of cubes together
class Chunk : public Mesh
{
private:
    bool Blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    bool IsWithinRange(Vector3f Vec);

public:
    Chunk(Vector3f Offset, int (&Heightmap)[160][160]);
    void CreateMesh();
    void Draw(Shader *MeshShader, bool isDepth);
};