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
            std::cout << "Error: Value " << Value << " is too large for " << " Size " << BitLength << std::endl;

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

void Vertex::Testing()
{
    auto toBinaryString = [](uint32_t value, int bitLength = 32) -> std::string
    {
        std::bitset<32> bs(value);                    // Create a bitset with 32 bits
        return bs.to_string().substr(32 - bitLength); // Extract and return the relevant number of bits
    };

    std::cout << "Value is " << toBinaryString(this->CondensedPos, 32) << std::endl;

    auto GetValue = [](uint32_t &Target, int StartPos, int BitLength)
    {
        uint Mask = (1u << BitLength) - 1u;
        return float((Target >> StartPos) & Mask);
    };

    float test = GetValue(this->CondensedPos, 0, 6);
    float best = GetValue(this->CondensedPos, 6, 6);
    float hest = GetValue(this->CondensedPos, 12, 6);

    std::cout << "Position: " << test << ", " << best << ", " << hest << std::endl;
    // std::cout << "Normal: " << GetValue(this->CondensedOther, 0, 2) - 2 << ", " << GetValue(this->CondensedOther, 2, 2) - 2 << ", " << GetValue(this->CondensedOther, 4, 2) - 2 << std::endl;
    // std::cout << "TextureCoords: " << GetValue(this->CondensedOther, 6, 1) << ", " << GetValue(this->CondensedOther, 7, 1) << std::endl;
    // std::cout << "TextureIndex: " << GetValue(this->CondensedOther, 8, 8) << std::endl;
}

Vector2f Vertex::GetTextureCoordinates()
{
    auto GetValue = [](uint32_t &Target, int StartPos, int BitLength)
    {
        uint Mask = (1u << BitLength) - 1u;
        return float((Target >> StartPos) & Mask);
    };

    return Vector2f(GetValue(this->CondensedOther, 6, 1), GetValue(this->CondensedOther, 7, 1));
}

Vector3f Vertex::GetNormal()
{
    auto GetValue = [](uint32_t &Target, int StartPos, int BitLength)
    {
        uint Mask = (1u << BitLength) - 1u;
        return float((Target >> StartPos) & Mask);
    };

    return Vector3f(GetValue(this->CondensedOther, 0, 2) - 2, GetValue(this->CondensedOther, 2, 2) - 2, GetValue(this->CondensedOther, 4, 2) - 2);
}

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
    this->InitaliseData(&VAO, &VBO, &EBO, &Vertices, &Indices);
}

void Mesh::Draw(Shader *MeshShader, bool isDepth)
{
    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, this->Indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::InitaliseData(unsigned int *TempVAO, unsigned int *TempVBO, unsigned int *TempEBO, std::vector<Vertex> *TempVertices, std::vector<unsigned int> *TempIndices)
{
    // Create and bind VAO (Stores buffer + vertex information)
    glGenVertexArrays(1, TempVAO);
    glBindVertexArray(*TempVAO);

    glGenBuffers(1, TempVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *TempVBO);
    glBufferData(GL_ARRAY_BUFFER, TempVertices->size() * sizeof(Vertex), TempVertices, GL_STATIC_DRAW);

    glGenBuffers(1, TempEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *TempEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TempIndices->size() * sizeof(unsigned int), TempIndices, GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // glEnableVertexAttribArray(0); // Position

    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, Normal)));
    // glEnableVertexAttribArray(1); // Normals

    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, TextureCoordinates)));
    // glEnableVertexAttribArray(2); // Texture Coordinates

    // glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, TextureIndex)));
    // glEnableVertexAttribArray(3); // Texture Index
}

unsigned int Mesh::GetVAO() const
{
    return VAO;
}