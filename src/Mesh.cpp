#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "../include/Matrix.h"

#include "../include/Mesh.h"

Mesh::Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices)
{
    this->Vertices = Vertices;
    this->Indices = Indices;

    this->CreateMesh();
}

Mesh::Mesh()
{
    this->CreateMesh();
}

void Mesh::CreateMesh()
{
    this->InitaliseData();
}

void Mesh::Draw(Shader *MeshShader, bool isDepth)
{
    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, this->Indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::InitaliseData()
{
    // Create and bind VAO (Stores buffer + vertex information)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &this->Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &this->Indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0); // Position

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, Normal)));
    glEnableVertexAttribArray(1); // Normals

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, TextureCoordinates)));
    glEnableVertexAttribArray(2); // Texture Coordinates

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, TextureIndex)));
    glEnableVertexAttribArray(3); // Texture Index
}