#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <random>

#include "Chunk.h"

class Tree
{
private:
    unsigned int VAO, VBO, EBO;

    unsigned int LeafVAO, LeafVBO, LeafEBO;
    unsigned int TextureAtlasId;

    std::vector<ThinVertex> VertexData;
    std::vector<unsigned int> Indices;
    std::vector<Matrix4f> PositionsList;
    std::vector<Vector3f> FaceVertices;

    std::vector<ThinVertex> LeavesVertexData;
    std::vector<unsigned int> LeavesIndices;
    std::vector<Matrix4f> LeavesPositionsList;
    std::vector<Vector3f> LeavesFaceVertices;

    std::vector<Vector2f> TextureCoordinatesList = {Vector2f(0, 0), Vector2f(1, 0), Vector2f(1, 1), Vector2f(0, 1)};

    void CreateMesh()
    {
        // Generate trunks of tree
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(7, 15);

        auto height = distr(gen);
        height = 7;

        unsigned int indexer = 0;
        for (unsigned int i = 0; i < height; i++)
        {
            for (Vector3f Direction : DirectionsList)
            {
                auto [CubeFaceVertices, CubeFaceIndices] = GetCubeData(Direction, Vector3f(0, 0, 0));

                for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
                    VertexData.push_back(ThinVertex(CubeFaceVertices[i], TextureCoordinatesList[i]));

                std::for_each(CubeFaceIndices.begin(), CubeFaceIndices.end(), [indexer](unsigned int &index)
                              { index += 4 * indexer; });

                for (auto index : CubeFaceIndices)
                    Indices.push_back(index);

                FaceVertices.push_back(Direction);

                indexer += 1;
            }

            Matrix4f ModelMatrix = Matrix4f(1);
            ModelMatrix.Translate(Vector3f(0, i, 0));
            PositionsList.push_back(ModelMatrix);
        }

        // Generate leaves
        // TODO: Randomise the numbers within
        indexer = 0;
        for (int i = -5; i < 5; i++)
        {
            for (int j = -5; j < 5; j++)
            {
                for (Vector3f Direction : DirectionsList)
                {
                    auto [CubeFaceVertices, CubeFaceIndices] = GetCubeData(Direction, Vector3f(0, 0, 0));

                    for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
                        LeavesVertexData.push_back(ThinVertex(CubeFaceVertices[i], TextureCoordinatesList[i]));

                    std::for_each(CubeFaceIndices.begin(), CubeFaceIndices.end(), [indexer](unsigned int &index)
                                  { index += 4 * indexer; });

                    for (auto index : CubeFaceIndices)
                        LeavesIndices.push_back(index);

                    LeavesFaceVertices.push_back(Direction);

                    indexer += 1;
                }

                Matrix4f ModelMatrix = Matrix4f(1);
                ModelMatrix.Translate(Vector3f(i, height, j));
                LeavesPositionsList.push_back(ModelMatrix);
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

        glGenVertexArrays(1, &LeafVAO);
        glBindVertexArray(LeafVAO);

        glGenBuffers(1, &LeafVBO);
        glBindBuffer(GL_ARRAY_BUFFER, LeafVBO);
        glBufferData(GL_ARRAY_BUFFER, LeavesVertexData.size() * sizeof(ThinVertex), &LeavesVertexData[0], GL_STATIC_DRAW);

        glGenBuffers(1, &LeafEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LeafEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, LeavesIndices.size() * sizeof(unsigned int), &LeavesIndices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // Setup position attribute
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))); // Setup position attribute
        glEnableVertexAttribArray(1);
    }

public:
    Tree()
    {
        // Setup textures
        TextureAtlasId = LoadTextureFromFile("TesturePack2.png");
        CreateMesh();
    }

    void Draw(Shader *MeshShader)
    {
        MeshShader->SetFloat("NumberOfRows", 16.0f);
        for (unsigned int i = 0; i < PositionsList.size(); i++)
        {
            MeshShader->SetMatrix4f("model", (const float *)(&PositionsList[i]));

            glActiveTexture(GL_TEXTURE0);
            MeshShader->SetInt("DiffuseTexture0", 0);
            glBindTexture(GL_TEXTURE_2D, TextureAtlasId);

            glBindVertexArray(VAO);

            MeshShader->SetFloat("TestIndex", 244.0f);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)((i * 36) * sizeof(GLuint)));
        }

        // std::cout << PositionsList.size() * 36 << std::endl;
        // std::cout << LeavesPositionsList.size() << std::endl;

        for (unsigned int i = 0; i < LeavesPositionsList.size(); i++)
        {
            MeshShader->SetMatrix4f("model", (const float *)(&LeavesPositionsList[i]));

            glActiveTexture(GL_TEXTURE0);
            MeshShader->SetInt("DiffuseTexture0", 0);
            glBindTexture(GL_TEXTURE_2D, TextureAtlasId);

            glBindVertexArray(LeafVAO);

            MeshShader->SetFloat("TestIndex", 246.0f);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)((i * 36) * sizeof(GLuint)));
        }
    }
};
