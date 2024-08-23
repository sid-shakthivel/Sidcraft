#pragma once

enum BlockType : unsigned char
{
    Air = 0,
    Grass = 1,
    Dirt = 2,
    Stone = 3,
    Water = 4,
    Sand = 5,

    TreeTrunk = 6,
    TreeLeaves = 7,
    Flower = 8,
    TallGrass = 9
};

const unsigned int CHUNK_NUM = 15;
constexpr unsigned int WORLD_SIZE = 16 * CHUNK_NUM;