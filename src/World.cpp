#include <vector>
#include <random>
#include <map>

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
    GenerateWorld();
}

void World::GenerateWorld()
{
    for (int i = 0; i < 1; i++)
    {
        for (int j = 0; j < 1; j++)
        {
            Matrix4f ModelMatrix = Matrix4f(1);
            ModelMatrix.Translate(Vector3f(i * CHUNK_SIZE, 0, (j + 1) * CHUNK_SIZE));

            Chunk NewChunk = Chunk(Vector3f(i, 0, j), Heightmap);
            NewChunk.CreateMesh();

            ChunkPositions.push_back(ModelMatrix);
            ChunkData.push_back(NewChunk);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> WorldRange(0, 159);
    // std::uniform_int_distribution<> WorldRange(0, 32);

    // Generate trees
    // for (int i = 0; i < 25; i++)
    // {
    //     auto PosX = WorldRange(gen);
    //     auto PosZ = WorldRange(gen);

    //     // PosX = 8;
    //     // PosZ = 8;

    //     auto Height = Heightmap[PosZ][PosX];

    //     Tree NewTree = Tree(Vector3f(PosX, Height, PosZ));
    //     NewTree.CreateMesh();
    //     TreeList.push_back(NewTree);
    // }s
}
