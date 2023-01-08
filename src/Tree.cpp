#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <random>

#include "../include/Mesh.h"
#include "../include/Cube.h"
#include "../include/Shader.h"

#include "../include/Tree.h"

Tree::Tree(Vector3f Offset) : CubeData(cube.GetCubeData())
{
    this->Offset = Offset;
}

void Tree::Draw(Shader *MeshShader, bool isDepth)
{
    glBindVertexArray(VAO);

    // if (!isDepth)
    // MeshShader->SetFloat("TestIndex", 4.0f);

    for (unsigned int i = 0; i < PositionsList.size(); i++)
    {
        // PositionsList[i].Print();
        MeshShader->SetMatrix4f("model", (const float *)(&PositionsList[i]));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)((0) * sizeof(GLuint)));
    }

    // if (!isDepth)
    // MeshShader->SetFloat("TestIndex", 6.0f);

    // for (unsigned int i = 0; i < LeavesPositionList.size(); i++)
    // {
    //     MeshShader->SetMatrix4f("model", (const float *)(&LeavesPositionList[i]));
    //     glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)((0) * sizeof(GLuint)));
    // }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0.0));
    model = glm::scale(model, glm::vec3(2.0f));
    MeshShader->SetMatrix4f("model", (const float *)(&model));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)((0) * sizeof(GLuint)));

    glBindVertexArray(0);
}

void Tree::CreateMesh()
{
    // Generate trunks of tree
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> TrunkRange(7, 15);

    auto TrunkHeight = TrunkRange(gen);

    // for (unsigned int i = 0; i < TrunkHeight; i++)
    // {
    //     Matrix4f ModelMatrix = Matrix4f(1);
    //     ModelMatrix.Translate(Vector3f(0 + Offset.x, i + Offset.y, 0 + Offset.z));
    //     PositionsList.push_back(ModelMatrix);
    // }

    for (int i = -5; i < 5; i++)
    {
        for (int j = -5; j < 5; j++)
        {
            Matrix4f ModelMatrix = Matrix4f(1);
            ModelMatrix.Translate(Vector3f(i + Offset.x, 0, j + Offset.z));
            PositionsList.push_back(ModelMatrix);
        }
    }

    // Generate leaves
    std::uniform_int_distribution<> LeafRange(5, 7);

    auto LeavesLength = LeafRange(gen) / 2;
    auto LeavesWidth = LeafRange(gen) / 2;
    auto LeavesHeight = LeafRange(gen) / 2;

    // for (int i = -LeavesLength; i < LeavesLength; i++)
    //     for (int j = -LeavesWidth; j < LeavesWidth; j++)
    //         for (int k = 0; k < LeavesHeight; k++)
    //         {
    //             Matrix4f ModelMatrix = Matrix4f(1);
    //             ModelMatrix.Translate(Vector3f(i + Offset.x, Offset.y + k + 5, j + Offset.z));
    //             // ModelMatrix.Translate(Vector3f(i + Offset.x, 5 + Offset.y + k, j + Offset.z));
    //             LeavesPositionList.push_back(ModelMatrix);
    //         }

    // Matrix4f ModelMatrix = Matrix4f(1);
    // ModelMatrix.Translate(Vector3f(i + Offset.x, Offset.y + k + 5, j + Offset.z));
    // LeavesPositionList.push_back(ModelMatrix);

    Vertices = CubeData.Vertices;
    Faces = CubeData.Faces;
    Indices = CubeData.Indices;

    Mesh::CreateMesh();

    // std::cout << VAO << std::endl;
    // std::cout << Vertices.size() << std::endl;
    // std::cout << Faces.size() << std::endl;
    // std::cout << Indices.size() << std::endl;
}