#pragma once

#include "Matrix.h"
#include "Chunk.h"
#include "Skybox.h"
#include "Block.h"
#include "Cube.h"
#include "Vegetation.h"
#include "Tree.h"

#include <map>
#include <vector>

enum EntityType
{
    TreeThing,
    Lightbox,
};

class World
{
private:
    void GenerateWorld();

    void GenerateEntities(EntityType Entity, unsigned int Limit);

protected:
    World();

    static World *World_;

public:
    World(World &other) = delete;
    void operator=(const World &) = delete;

    static World *GetInstance();

    std::vector<Chunk> ChunkData;

    std::vector<Tree> TreeList;
    std::vector<Vegetation> FlowerList;
    std::vector<Cube> LightCubes;

    std::map<BlockType, int> Inventory;

    int Heightmap[WORLD_SIZE][WORLD_SIZE];

    Skybox skybox;
};