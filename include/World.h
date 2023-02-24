#pragma once

#include "Matrix.h"
#include "Tree.h"
#include "Chunk.h"
#include "Skybox.h"
#include "Block.h"
#include "Cube.h"
#include "Vegetation.h"

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
    std::vector<Vegetation> FlowerList;
    std::vector<Cube> LightCubes;
    std::vector<Matrix4f> LightCubePositions;

    std::map<BlockType, int> Inventory;

    int Heightmap[WORLD_SIZE][WORLD_SIZE];

    Skybox skybox;
};