#include <vector>

#include "../include/Cube.h"
#include "../include/Vegetation.h"

Vegetation::Vegetation(float TextureIndex, Vector3f TranslationVector)
{
    // Position.Translate(TranslationVector);

    // // Create a criss cross system
    // std::vector<Vector3f> FirstFaceVertices{
    //     Vector3f(0.5f, -0.5f, 0.5f),
    //     Vector3f(-0.5f, -0.5f, -0.5f),
    //     Vector3f(-0.5f, 0.5f, -0.5f),
    //     Vector3f(0.5f, 0.5f, 0.5f),
    // };

    // std::vector<Vector3f> SecondFaceVertices{
    //     Vector3f(-0.5f, -0.5f, 0.5f),
    //     Vector3f(0.5f, -0.5f, -0.5f),
    //     Vector3f(0.5f, 0.5f, -0.5f),
    //     Vector3f(-0.5f, 0.5f, 0.5f),
    // };

    // auto FirstFaceNormals = CaculateNormals(FirstFaceVertices);
    // auto SecondFaceNormals = CaculateNormals(SecondFaceVertices);

    // Indices.insert(Indices.end(), {0, 1, 2, 2, 3, 0});
    // Indices.insert(Indices.end(), {4, 5, 6, 6, 7, 4});

    // for (unsigned int i = 0; i < FirstFaceVertices.size(); i++)
    //     Vertices.push_back(Vertex(FirstFaceVertices[i], FirstFaceNormals[i], Cube::TextureCoordinatesList[i], TextureIndex));

    // for (unsigned int i = 0; i < SecondFaceVertices.size(); i++)
    //     Vertices.push_back(Vertex(SecondFaceVertices[i], SecondFaceNormals[i], Cube::TextureCoordinatesList[i], TextureIndex));

    // CreateMesh();
}

void Vegetation::Draw(Shader *MeshShader)
{
    // MeshShader->SetMatrix4f("Model", (const float *)(&Position));

    // glBindVertexArray(VAO);
    // glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, (void *)(0 * sizeof(GLuint)));
}

void Vegetation::CreateMesh()
{
    // Mesh::CreateMesh();

    // glGenVertexArrays(1, &VAO);
    // glBindVertexArray(VAO);

    // glGenBuffers(1, &VBO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

    // glGenBuffers(1, &EBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // glEnableVertexAttribArray(0); // Position

    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, Normal)));
    // glEnableVertexAttribArray(1); // Normals

    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, TextureCoordinates)));
    // glEnableVertexAttribArray(2); // Texture Coordinates

    // glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, TextureIndex)));
    // glEnableVertexAttribArray(3); // Texture Index
}