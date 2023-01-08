#include <vector>
#include <random>
#include <map>

#include "../include/World.h"

World::World()
{
    GenerateWorld();
}

void World::GenerateWorld()
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            Matrix4f ModelMatrix = Matrix4f(1);
            ModelMatrix.Translate(Vector3f(i * CHUNK_SIZE, 0, j * CHUNK_SIZE));

            Chunk NewChunk = Chunk(Vector3f(i, 0, j), Heightmap);
            NewChunk.CreateMesh();

            TerrainData.insert(std::make_pair(ModelMatrix, NewChunk));
        }
    }

    // std::cout << TerrainData.size() << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> WorldRange(0, 160);

    // Generate trees
    for (int i = 0; i < 25; i++)
    {
        auto PosX = WorldRange(gen);
        auto PosZ = WorldRange(gen);
        auto Height = Heightmap[PosZ][PosX];

        Tree NewTree = Tree(Vector3f(PosX, Height, PosZ));
        NewTree.CreateMesh();
        TreeList.push_back(NewTree);
    }
}
