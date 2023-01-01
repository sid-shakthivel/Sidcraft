#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <tuple>
#include <vector>
#include <string>

#include "matrix.h"
#include "shader.h"
#include "stb_image.h"
#include "mesh.h"
#include "PerlinNoise.hpp"

const Vector3f UP = Vector3f(0.0f, 1.0f, 0.0f);
const Vector3f DOWN = Vector3f(0.0f, -1.0f, 0.0f);
const Vector3f LEFT = Vector3f(-1.0f, 0.0f, 0.0f);
const Vector3f RIGHT = Vector3f(1.0f, 0.0f, 0.0f);
const Vector3f FRONT = Vector3f(0.0f, 0.0f, 1.0f);
const Vector3f BACK = Vector3f(0.0f, 0.0f, -1.0f);

static const unsigned int CHUNK_SIZE = 16;

struct ThinVertex
{
    Vector3f Position;
    Vector2f TextureCoordinates;

    ThinVertex(Vector3f Position, Vector2f TextureCoordinates)
    {
        this->Position = Position;
        this->TextureCoordinates = TextureCoordinates;
    }
};

unsigned int LoadTextureFromFile(const std::string &filepath)
{
    unsigned int TextureId;
    glGenTextures(1, &TextureId);
    glBindTexture(GL_TEXTURE_2D, TextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);          // (s, t, r) refers to the coordinate axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);          // Mirrors content
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Set filtering method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char *data = stbi_load((filepath).c_str(), &width, &height, &nrChannels, 0);

    if (!data)
        std::cout << "ERROR LOADING TEXTURE";

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return TextureId;
}

// Represents a number of blocks together
class Chunk
{
private:
    bool Blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    Texture *MainTexture;

    std::vector<ThinVertex> VertexData;
    std::vector<unsigned int> Indices;

    std::vector<Vector3f> DirectionsList = {UP, DOWN, LEFT, RIGHT, FRONT, BACK};
    std::vector<Vector2f> TextureCoordinatesList = {Vector2f(0, 0), Vector2f(1, 0), Vector2f(1, 1), Vector2f(0, 1)};

    unsigned int VAO, VBO, EBO;

    std::tuple<std::vector<Vector3f>, std::vector<unsigned int>> GetCubeData(Vector3f Direction, Vector3f Position)
    {
        std::vector<Vector3f> FaceVertices;
        std::vector<unsigned int> FaceIndices;

        if (Direction.IsEqual(UP))
        {
            FaceVertices.push_back(Vector3f(-0.5f, 0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, 0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, 0.5f, -0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, 0.5f, -0.5f));

            FaceIndices = {0, 1, 2, 2, 3, 0};
        }
        else if (Direction.IsEqual(DOWN))
        {
            FaceVertices.push_back(Vector3f(-0.5f, -0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, -0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, -0.5f, -0.5f));
            FaceVertices.push_back(Vector3f(0.5f, -0.5f, -0.5f));

            FaceIndices = {0, 1, 2, 2, 3, 1};
        }
        else if (Direction.IsEqual(LEFT))
        {
            FaceVertices.push_back(Vector3f(0.5f, 0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, -0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, 0.5f, -0.5f));
            FaceVertices.push_back(Vector3f(0.5f, -0.5f, -0.5f));

            FaceIndices = {0, 1, 2, 2, 3, 1};
        }
        else if (Direction.IsEqual(RIGHT))
        {
            FaceVertices.push_back(Vector3f(-0.5f, 0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, -0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, 0.5f, -0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, -0.5f, -0.5f));

            FaceIndices = {0, 1, 2, 2, 3, 1};
        }
        else if (Direction.IsEqual(FRONT))
        {
            FaceVertices.push_back(Vector3f(-0.5f, -0.5f, -0.5f));
            FaceVertices.push_back(Vector3f(0.5f, -0.5f, -0.5f));
            FaceVertices.push_back(Vector3f(0.5f, 0.5f, -0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, 0.5f, -0.5f));

            FaceIndices = {0, 1, 2, 2, 3, 0};
        }
        else
        {
            // Back
            FaceVertices.push_back(Vector3f(-0.5f, -0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, -0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(0.5f, 0.5f, 0.5f));
            FaceVertices.push_back(Vector3f(-0.5f, 0.5f, 0.5f));

            FaceIndices = {0, 1, 2, 2, 3, 0};
        }

        // Vertex positions must be offsetted by the position
        std::for_each(FaceVertices.begin(), FaceVertices.end(), [Position](auto &vertex)
                      { vertex = vertex.Add(Position); });

        return {FaceVertices, FaceIndices};
    }

    bool IsWithinRange(Vector3f Vec)
    {
        if (Vec.x < 0 || Vec.x >= CHUNK_SIZE || Vec.y < 0 || Vec.y >= CHUNK_SIZE || Vec.z < 0 || Vec.z >= CHUNK_SIZE)
            return true;
        return false;
    }

public:
    /*
        Determines whether a block should be rendered or not
        In future when we can delete blocks may come in handy
    */
    Chunk()
    {
        // Setup texture
        auto TextureId = LoadTextureFromFile("grass.png");
        auto Test = Texture(TextureId, "grass", "grass.png", TextureType::Diffuse);
        MainTexture = &Test;

        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int z = 0; z < CHUNK_SIZE; z++)
                for (int y = 0; y < CHUNK_SIZE; y++)
                    Blocks[x][y][z] = false;

        /*
            Uses perlin noise to determine height
            Build terrain by making each column a different / same height
        */

        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        const siv::PerlinNoise perlin{static_cast<unsigned int>(std::rand())};

        const float scale = 1.0f / 10.0f;

        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                const int height = perlin.normalizedOctave2D_01((z * scale), (x * scale), 8) * CHUNK_SIZE;

                for (int y = 0; y < height; y++)
                    Blocks[x][y][z] = true;
            }
    }

    void CreateMesh()
    {
        /*
            Loop through each block, and check if adjacent blocks are within the chunk
            If an adjacent block is within the chunk, it need not be rendered
            Else the adjacent block is an air block and thus this face might be visible so can be rendered
        */
        unsigned int indexer = 0;
        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int y = 0; y < CHUNK_SIZE; y++)
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    Vector3f Position = Vector3f(x, y, z);

                    for (Vector3f Direction : DirectionsList)
                    {
                        Vector3f PositionToCheck = Position.Add(Direction);

                        if (IsWithinRange(PositionToCheck) || Blocks[(int)PositionToCheck.x][(int)PositionToCheck.y][(int)PositionToCheck.z] == false)
                        {
                            if (Blocks[x][y][z] == true)
                            {
                                // Fix for Up/Down
                                if (Direction.IsEqual(UP) || Direction.IsEqual(DOWN))
                                    PositionToCheck = Position;

                                auto [CubeFaceVertices, CubeFaceIndices] = GetCubeData(Direction, PositionToCheck);

                                std::for_each(CubeFaceIndices.begin(), CubeFaceIndices.end(), [indexer](unsigned int &index)
                                              { index += 4 * indexer; });
                                for (auto index : CubeFaceIndices)
                                    Indices.push_back(index);

                                for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
                                    VertexData.push_back(ThinVertex(CubeFaceVertices[i], TextureCoordinatesList[i]));

                                indexer += 1;
                            }
                        }
                    }
                }

        // Setup rendering buffers
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, VertexData.size() * sizeof(ThinVertex), &VertexData[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // Setup position attribute
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))); // Setup position attribute
        glEnableVertexAttribArray(1);
    }

    void Draw(Shader *MeshShader)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, MainTexture->Id);
        MeshShader->SetInt("DiffuseTexture0", 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
    }
};