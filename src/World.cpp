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
    GenerateWorld();
}

void World::GenerateWorld()
{
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            Matrix4f ModelMatrix = Matrix4f(1);
            ModelMatrix.Translate(Vector3f(i * CHUNK_SIZE, 0, (j)*CHUNK_SIZE));

            Chunk NewChunk = Chunk(Vector3f(i, 0, j), Heightmap);
            NewChunk.CreateMesh();

            ChunkPositions.push_back(ModelMatrix);
            ChunkData.push_back(NewChunk);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> WorldRange(0, 239);

    // Generate trees
    for (int i = 0; i < 25; i++)
    {
        auto PosX = WorldRange(gen);
        auto PosZ = WorldRange(gen);

        auto Height = Heightmap[PosZ][PosX];

        if (Height <= WATER_LEVEL)
            continue;

        Tree NewTree = Tree(Vector3f(PosX, Height, PosZ));
        NewTree.CreateMesh();
        TreeList.push_back(NewTree);
    }

    // Generate flowers
    for (int i = 0; i < 50; i++)
    {
        auto PosX = WorldRange(gen);
        auto PosZ = WorldRange(gen);

        auto Height = Heightmap[PosZ][PosX];

        if (Height <= WATER_LEVEL)
            continue;

        Cube NewFlower = Cube(TextureAtlas::GetInstance()->FetchFlower());
        NewFlower.CreateMesh();
        FlowerList.push_back(NewFlower);

        Matrix4f ModelMatrix = Matrix4f(1);
        ModelMatrix.Translate(Vector3f(PosX, Height + 5, PosZ));
        FlowerPositions.push_back(ModelMatrix);
    }
}
