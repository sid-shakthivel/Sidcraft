#pragma once

#include "Matrix.h"
#include "Chunk.h"
#include "Skybox.h"
#include "Block.h"
#include "Cube.h"

#include <map>
#include <vector>

class World
{
private:
    void GenerateWorld();

protected:
    World();

    static World *World_;

    unsigned int VAO;

public:
    World(World &other) = delete;
    ~World();
    void operator=(const World &) = delete;

    static World *GetInstance();

    std::vector<Chunk *> ChunkData;

    std::vector<Cube> LightCubes;

    std::map<BlockType, int> Inventory;

    int Heightmap[WORLD_SIZE][WORLD_SIZE];

    Skybox skybox;
};