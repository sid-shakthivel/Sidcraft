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

void World::GenerateEntities(EntityType Entity, unsigned int Limit)
{
    unsigned int EntityCount = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> WorldRange(0, WORLD_SIZE - 1);

    while (EntityCount < Limit)
    {
        auto PosX = WorldRange(gen);
        auto PosZ = WorldRange(gen);

        auto Height = Heightmap[PosZ][PosX];

        Vector3f TranslationVector = Vector3f(PosX, Height + 1, PosZ);

        if (Height > WATER_LEVEL)
        {
            switch (Entity)
            {
            case EntityType::Tree:
            {
                TestTree NewTree = TestTree(TranslationVector);
                TreeList.push_back(NewTree);
                break;
            }
            case EntityType::Lightbox:
            {
                Cube NewLightCube = Cube(35.0f, TranslationVector);
                LightCubes.push_back(NewLightCube);
                break;
            }
            case EntityType::Flower:
            {
                Vegetation NewFlower = Vegetation(TextureAtlas::GetInstance()->FetchFlower(), TranslationVector);
                FlowerList.push_back(NewFlower);
                break;
            }
            }

            EntityCount++;
        }
    }
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

            ChunkPositions.push_back(ModelMatrix);
            ChunkData.push_back(NewChunk);
        }
    }

    GenerateEntities(EntityType::Tree, 25);
    GenerateEntities(EntityType::Flower, 50);
    GenerateEntities(EntityType::Lightbox, 10);
}
