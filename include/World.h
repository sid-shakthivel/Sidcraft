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

public:
    std::vector<Tree> TreeList;
    int Heightmap[160][160];
    std::map<Matrix4f, Chunk> TerrainData;
    Skybox skybox;

    World();
};