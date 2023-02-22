#include <vector>
#include <random>
#include <map>

#include "../include/Chunk.h"
#include "../include/TextureAtlas.h"
#include "../include/Vegetation.h"
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
    for (int i = 0; i < CHUNK_NUM; i++)
    {
        for (int j = 0; j < CHUNK_NUM; j++)
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
    std::uniform_int_distribution<> WorldRange(0, WORLD_SIZE - 1);

    // Generate trees
    // unsigned int TreeCount = 0;

    // for (;;)
    // {
    //     auto PosX = WorldRange(gen);
    //     auto PosZ = WorldRange(gen);

    //     auto Height = Heightmap[PosZ][PosX];

    //     if (Height > WATER_LEVEL)
    //     {
    //         Tree NewTree = Tree(Vector3f(PosX, Height, PosZ));
    //         NewTree.CreateMesh();
    //         TreeList.push_back(NewTree);

    //         TreeCount += 1;
    //     }

    //     if (TreeCount >= CHUNK_NUM * 3)
    //         break;
    // }

    // Generate flowers
    // unsigned int FlowerCount = 0;

    // for (;;)
    // {
    //     auto PosX = WorldRange(gen);
    //     auto PosZ = WorldRange(gen);
    
    //     auto Height = Heightmap[PosZ][PosX];

    //     if (Height > WATER_LEVEL)
    //     {
    //         Vegetation NewFlower = Vegetation(TextureAtlas::GetInstance()->FetchFlower());
    //         NewFlower.CreateMesh();
    //         FlowerList.push_back(NewFlower);

    //         Matrix4f ModelMatrix = Matrix4f(1);
    //         ModelMatrix.Translate(Vector3f(PosX, Height + 1, PosZ));
    //         FlowerPositions.push_back(ModelMatrix);

    //         FlowerCount += 1;
    //     }

    //     if (FlowerCount >= CHUNK_NUM * 3)
    //         break;
    // }
}
