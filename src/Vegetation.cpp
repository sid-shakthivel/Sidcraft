#include <vector>

#include "../include/Vegetation.h"

Vegetation::Vegetation(float TextureIndex)
{
    // Create a criss cross system
    std::vector<Vector3f> FirstFaceVertices{
        Vector3f(0.5f, -0.5f, 0.5f),
        Vector3f(-0.5f, -0.5f, -0.5f),
        Vector3f(-0.5f, 0.5f, -0.5f),
        Vector3f(0.5f, 0.5f, 0.5f),
    };

    std::vector<Vector3f> SecondFaceVertices{
        Vector3f(-0.5f, -0.5f, 0.5f),
        Vector3f(0.5f, -0.5f, -0.5f),
        Vector3f(0.5f, 0.5f, -0.5f),
        Vector3f(-0.5f, 0.5f, 0.5f),
    };

    auto FirstFaceNormals = CaculateNormals(FirstFaceVertices);
    auto SecondFaceNormals = CaculateNormals(SecondFaceVertices);

    Indices.insert(Indices.end(), {0, 1, 2, 2, 3, 0});
    Indices.insert(Indices.end(), {4, 5, 6, 6, 7, 4});

    for (unsigned int i = 0; i < FirstFaceVertices.size(); i++)
        Vertices.push_back(Vertex(FirstFaceVertices[i], FirstFaceNormals[i], TextureCoordinatesList[i], TextureIndex));

    for (unsigned int i = 0; i < SecondFaceVertices.size(); i++)
        Vertices.push_back(Vertex(SecondFaceVertices[i], SecondFaceNormals[i], TextureCoordinatesList[i], TextureIndex));
}

void Vegetation::Draw(Shader *MeshShader, Matrix4f Offset) const
{
    MeshShader->SetMatrix4f("model", (const float *)(&Offset));
    MeshShader->SetFloat("PerlinOffset", 1.0f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, (void *)(0 * sizeof(GLuint)));
}

void Vegetation::CreateMesh()
{
    Mesh::CreateMesh();
}