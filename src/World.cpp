#include <vector>
#include <random>
#include <map>

#include "../include/Chunk.h"
#include "../include/TextureAtlas.h"
#include "../include/World.h"

World *World::World_ = nullptr;

World *World::GetInstance()
{
    if (World_ == nullptr)
        World_ = new World();
    return World_;
}

World::World()
{
    std::fill(&Heightmap[0][0], &Heightmap[0][0] + WORLD_SIZE * WORLD_SIZE, 0);
    GenerateWorld();
}

World::~World()
{
    for (int i = 0; i < ChunkData.size(); i++)
        delete ChunkData.at(i);
}

void World::GenerateWorld()
{
    // Setup a central VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    for (int i = 0; i < CHUNK_NUM; i++)
        for (int j = 0; j < CHUNK_NUM; j++)
            ChunkData.push_back(new Chunk(Vector3f(i, 0, j), VAO, Heightmap));
}
