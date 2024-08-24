#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "../include/Matrix.h"

#include "../include/Mesh.h"

Vertex::Vertex(Vector3f Position, Vector3f Normal, Vector2f TextureCoordinates, float TextureIndex)
{
    auto InsertValue = [](uint32_t &Target, int Value, int StartPos, int BitLength)
    {
        if (Value >= (1u << BitLength))
        {
            std::cout << "Error: Value " << Value << " is too large for " << " Size " << BitLength << std::endl;
            std::exit(0);
        }

        Target |= (Value << StartPos);
    };

    this->CondensedPos = 0;
    this->CondensedOther = 0;

    InsertValue(this->CondensedPos, Position.x, 0, 6);
    InsertValue(this->CondensedPos, Position.y, 6, 6);
    InsertValue(this->CondensedPos, Position.z, 12, 6);

    // Potentially negative hence update
    InsertValue(this->CondensedOther, Normal.x + 2, 0, 2);
    InsertValue(this->CondensedOther, Normal.y + 2, 2, 2);
    InsertValue(this->CondensedOther, Normal.z + 2, 4, 2);

    InsertValue(this->CondensedOther, TextureCoordinates.x, 6, 1);
    InsertValue(this->CondensedOther, TextureCoordinates.y, 7, 1);

    InsertValue(this->CondensedOther, TextureIndex, 8, 8);
}

Mesh::Mesh()
{
    Vertices = new std::vector<Vertex>();
    Indices = new std::vector<unsigned int>();
}

Mesh::~Mesh()
{
    if (VAO)
        glDeleteBuffers(1, &VAO);

    if (VBO)
        glDeleteBuffers(1, &VBO);

    if (EBO)
        glDeleteBuffers(1, &EBO);

    // Cleanup
    // delete Vertices;
    // delete Indices;
}

void Mesh::Initialise()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    Populate();
}

void Mesh::Initialise(unsigned int SuppliedVAO)
{
    VAO = SuppliedVAO;
    glBindVertexArray(VAO);

    Populate();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::Populate()
{
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices->size() * sizeof(Vertex), Vertices->data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices->size() * sizeof(unsigned int), Indices->data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void *)(offsetof(Vertex, CondensedPos)));
    glEnableVertexAttribArray(0);

    // Other data including Normals, TextureCoords, TextureIndex
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void *)(offsetof(Vertex, CondensedOther)));
    glEnableVertexAttribArray(1);
}

void Mesh::SetModel(Matrix4f Model)
{
    ModelMatrix = Model;
}

void Mesh::Draw(Shader *MeshShader)
{
    // Draw mesh
    glBindVertexArray(VAO);
    MeshShader->SetMatrix4f("Model", (const float *)(&ModelMatrix));
    glDrawElements(GL_TRIANGLES, this->Indices->size(), GL_UNSIGNED_INT, 0);
}

std::vector<Vector3f> Mesh::CaculateNormals(std::array<Vector3f, 4> &Vertices)
{
    std::vector<Vector3f> NormalsList;

    auto Tri1Corn1 = Vertices[0];
    auto Tri1Corn2 = Vertices[1];
    auto Tri1Corn3 = Vertices[2];

    auto Tri2Corn1 = Vertices[2];
    auto Tri2Corn2 = Vertices[3];
    auto Tri2Corn3 = Vertices[0];

    return NormalsList;
}