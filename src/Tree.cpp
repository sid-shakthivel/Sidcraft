#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <random>

#include "../include/Mesh.h"
#include "../include/Cube.h"
#include "../include/Shader.h"
#include "../include/TextureAtlas.h"

#include "../include/Tree.h"

TestTree::TestTree(Vector3f Offset)
{
    this->Offset = Offset;

    TrunkCube = Cube(TextureAtlas::GetInstance()->FetchTreeTrunk());
    LeafCube = Cube(TextureAtlas::GetInstance()->FetchTreeLeaves());

    // Generate trunks of tree
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> TrunkRange(5, 7);

    auto TrunkHeight = TrunkRange(gen);

    for (unsigned int i = 0; i < TrunkHeight; i++)
    {
        Matrix4f ModelMatrix = Matrix4f(1);
        ModelMatrix.Translate(Vector3f(0 + Offset.x, i + Offset.y, 0 + Offset.z));
        PositionsList.push_back(ModelMatrix);
    }

    // Generate leaves
    std::uniform_int_distribution<> LeafRange(2, 4);

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
}

void TestTree::DrawTrunk(Shader *MeshShader) const
{
    glBindVertexArray(TrunkCube.GetVAO());

    for (unsigned int i = 0; i < PositionsList.size(); i++)
    {
        MeshShader->SetMatrix4f("Model", (const float *)(&PositionsList[i]));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)((0) * sizeof(GLuint)));
    }
}

void TestTree::DrawLeaves(Shader *MeshShader) const
{
    glBindVertexArray(LeafCube.GetVAO());

    for (unsigned int i = 0; i < LeavesPositionList.size(); i++)
    {
        MeshShader->SetMatrix4f("Model", (const float *)(&LeavesPositionList[i]));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)((0) * sizeof(GLuint)));
    }
}

void TestTree::CreateMesh() {}