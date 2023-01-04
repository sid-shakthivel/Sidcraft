#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <random>

#include "Chunk.h"

struct MeshData
{
    std::vector<ThinVertex> VertexData;
    std::vector<unsigned int> Indices;
    std::vector<Vector3f> FaceVertices;

    MeshData(std::vector<ThinVertex> VertexData, std::vector<unsigned int> Indices, std::vector<Vector3f> FaceVertices)
    {
        this->VertexData = VertexData;
        this->Indices = Indices;
        this->FaceVertices = FaceVertices;
    }
};

class Cube
{
private:
    std::vector<ThinVertex> VertexData;
    std::vector<unsigned int> Indices;
    std::vector<Vector3f> FaceVertices;

    std::vector<Vector2f> TextureCoordinatesList = {Vector2f(0, 0), Vector2f(1, 0), Vector2f(1, 1), Vector2f(0, 1)};

public:
    Cube()
    {
        /*
            Simple generates a singular cube
        */
        unsigned int indexer = 0;
        for (Vector3f Direction : DirectionsList)
        {
            auto [CubeFaceVertices, CubeFaceIndices] = ::GetCubeData(Direction, Vector3f(0, 0, 0));

            for (unsigned int i = 0; i < CubeFaceVertices.size(); i++)
                VertexData.push_back(ThinVertex(CubeFaceVertices[i], TextureCoordinatesList[i]));

            std::for_each(CubeFaceIndices.begin(), CubeFaceIndices.end(), [indexer](unsigned int &index)
                          { index += 4 * indexer; });

            for (auto index : CubeFaceIndices)
                Indices.push_back(index);

            FaceVertices.push_back(Direction);

            indexer += 1;
        }
    }

    MeshData GetCubeData()
    {
        return MeshData(VertexData, Indices, FaceVertices);
    }
};

class Tree
{
private:
    unsigned int VAO, VBO, EBO;

    Cube cube = Cube();
    MeshData CubeData;
    std::vector<Matrix4f> PositionsList;
    std::vector<Matrix4f> LeavesPositionList;

    void CreateMesh(Vector3f Offset)
    {
        // Generate trunks of tree
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> TrunkRange(15, 20);

        auto TrunkHeight = TrunkRange(gen);

        for (unsigned int i = 0; i < TrunkHeight; i++)
        {
            Matrix4f ModelMatrix = Matrix4f(1);
            ModelMatrix.Translate(Vector3f(0 + Offset.x, i + Offset.y, 0 + Offset.z));
            PositionsList.push_back(ModelMatrix);
        }

        // Generate leaves
        std::uniform_int_distribution<> LeafRange(5, 7);

        auto LeavesLength = LeafRange(gen);
        auto LeavesWidth = LeafRange(gen);
        auto LeavesHeight = LeafRange(gen);

        for (int i = -LeavesLength; i < LeavesLength; i++)
            for (int j = -LeavesWidth; j < LeavesWidth; j++)
                for (int k = 0; k < LeavesHeight; k++)
                {
                    Matrix4f ModelMatrix = Matrix4f(1);
                    ModelMatrix.Translate(Vector3f(i + Offset.x, TrunkHeight + Offset.y + k, j + Offset.z));
                    LeavesPositionList.push_back(ModelMatrix);
                }

        // Setup rendering buffers
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, CubeData.VertexData.size() * sizeof(ThinVertex), &CubeData.VertexData[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, CubeData.Indices.size() * sizeof(unsigned int), &CubeData.Indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // Setup position attribute
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))); // Setup position attribute
        glEnableVertexAttribArray(1);
    }

public:
    Tree(Vector3f Offset) : CubeData(cube.GetCubeData())
    {
        CreateMesh(Offset);
    }

    void Draw(Shader *MeshShader)
    {
        MeshShader->SetFloat("TestIndex", 4.0f);
        glBindVertexArray(VAO);

        for (unsigned int i = 0; i < PositionsList.size(); i++)
        {
            MeshShader->SetMatrix4f("model", (const float *)(&PositionsList[i]));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)((0) * sizeof(GLuint)));
        }

        MeshShader->SetFloat("TestIndex", 6.0f);

        for (unsigned int i = 0; i < LeavesPositionList.size(); i++)
        {
            MeshShader->SetMatrix4f("model", (const float *)(&LeavesPositionList[i]));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)((0) * sizeof(GLuint)));
        }
    }
};
