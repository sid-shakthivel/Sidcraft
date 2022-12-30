#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

struct Vertex
{
    Vector3f Position;
    Vector3f Normal;
    Vector2f TextureCoordinates;
};

enum TextureType
{
    Diffuse,
    Specular
};

struct Texture
{
    unsigned int Id;
    std::string Name;
    std::string Path;
    TextureType Type;

    Texture(unsigned int Id, const std::string &Name, const std::string &Path, TextureType Type)
    {
        this->Id = Id;
        this->Name = Name;
        this->Path = Path;
        this->Type = Type;
    }
};

class Mesh
{
private:
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Texture> Textures;
    unsigned int VAO, VBO, EBO;

public:
    Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices, std::vector<Texture> Textures)
    {
        this->Vertices = Vertices;
        this->Indices = Indices;
        this->Textures = Textures;

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
    }

    void Draw(Shader *MeshShader)
    {
        for (int i = 0; i < this->Textures.size(); i++)
        {
            switch (this->Textures[i].Type)
            {
            case Diffuse:
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, this->Textures[i].Id);
                MeshShader->SetInt("DiffuseTexture" + std::to_string(i), i);
                break;
            case Specular:
                // MeshShader->SetInt("SpecularTexture" + std::to_string(i), i);
                break;
            }
        }

        // Draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, this->Indices.size(), GL_UNSIGNED_INT, 0);
    }
};