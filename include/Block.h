#pragma once

enum BlockType
{
    Air = 0,
    Grass = 1,
    Dirt = 2,
    Stone = 3,
    Water = 4,
    Sand = 5,
};

const unsigned int CHUNK_NUM = 1;
constexpr unsigned int WORLD_SIZE = 16 * CHUNK_NUM;