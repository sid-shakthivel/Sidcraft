#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "matrix.h"
#include "shader.h"

struct Vertex
{
    Vector3f Position;
    Vector3f Normal;
    Vector2f TextureCoordinates;
}

enum TextureType {
    Diffuse,
    Specular
}

struct Texture
{
    unsigned int Id;
    std::string Name;
    TextureType Type;
}

class Mesh
{
private:
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Texture> Textures;
    unsigned int VAO, VBO, EBO;

public:
    Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices, std::vector<Vertex> Textures)
    {
        this->Vertices = Vertices;
        this->Indices = Indices;
        this->Textures = Textures;

        // Create and bind VAO (Stores buffer + vertex information)
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0); // Position

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, Normal)));
        glEnableVertexAttribArray(1); // Normals

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, TextureCoordinates)));
        glEnableVertexAttribArray(2); // Texture Coordinates

        glBindVertexArray(VAO);
    }

    Draw(Shader *MeshShader)
    {
        for (int i = 0; i < this->Textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, this->Textures[i].Id);

            switch (this->Textures[i].Type)
            {
            case Diffuse:
                MeshShader->SetInt("DiffuseTexture" + std::to_string(i), i);
                break;
            case Specular:
                MeshShader->SetInt("SpecularTexture" + std::to_string(i), i);
                break;
            }
        }

        glActiveTexture(GL_TEXTURE0);

        // Draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, this->Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // Cleanup
    }
}