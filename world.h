#include "Chunk.h"

#include <map>

namespace World
{
    // Maps a Chunk to a position which is used for the Model matrix

    std::tuple<std::vector<Chunk>, std::vector<Matrix4f>> GenerateWorld()
    {
        std::vector<Chunk> ChunkList;
        std::vector<Matrix4f> PositionList;

        for (int i = -5; i < 5; i++)
        {
            for (int j = -5; j < 5; j++)
            {
                Chunk NewChunk = Chunk();
                ChunkList.emplace_back(NewChunk);

                Matrix4f ModelMatrix = Matrix4f(1);
                ModelMatrix.Translate(Vector3f(i * CHUNK_SIZE, 0, j * CHUNK_SIZE));

                PositionList.emplace_back(ModelMatrix);
            }
        }

        return {ChunkList, PositionList};
    }
}