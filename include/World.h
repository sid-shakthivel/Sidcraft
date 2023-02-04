#pragma once

#include "Matrix.h"
#include "Tree.h"
#include "Chunk.h"
#include "Skybox.h"

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
    std::map<Matrix4f, Chunk> TerrainData;

    int Heightmap[160][160];

    Skybox skybox;
};