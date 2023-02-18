#pragma once

#include "Matrix.h"
#include "Tree.h"
#include "Chunk.h"
#include "Skybox.h"
#include "Block.h"

#include <map>
#include <vector>

class World
{
private:
    void GenerateWorld();

protected:
    World();

    static World *World_;

public:
    World(World &other) = delete;
    void operator=(const World &) = delete;

    static World *GetInstance();

    std::vector<Tree> TreeList;
    std::vector<Matrix4f> ChunkPositions;
    std::vector<Chunk> ChunkData;

    std::map<BlockType, int> Inventory;

    int Heightmap[240][240];

    Skybox skybox;
};