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
        // for (int i = 0; i < this->Textures.size(); i++)
        // {
        //     switch (this->Textures[i].Type)
        //     {
        //     case Diffuse:
        //         glActiveTexture(GL_TEXTURE0 + i);
        //         glBindTexture(GL_TEXTURE_2D, this->Textures[i].Id);
        //         MeshShader->SetInt("DiffuseTexture" + std::to_string(i), i);
        //         break;
        //     }
        //     // case Specular:
        //     //     MeshShader->SetInt("SpecularTexture" + std::to_string(i), i);
        //     //     break;
        // }

        Matrix4f ProjectionMatrix = CreatePerspectiveProjectionMatrix(45.0f * 3.14159 / 180, 800.0f / 600.0f, 0.1f, 100.0f);
        Matrix4f ViewMatrix = Matrix4f(1);
        Matrix4f ModelMatrix = Matrix4f(1);
        ModelMatrix.Translate(Vector3f(0, 0, -10.0f));

        MeshShader->Use();
        MeshShader->SetMatrix4f("model", (const float *)(&ModelMatrix));
        MeshShader->SetMatrix4f("view", (const float *)(&ViewMatrix));
        MeshShader->SetMatrix4f("projection", (const float *)(&ProjectionMatrix));

        // glActiveTexture(GL_TEXTURE0);

        // Draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, this->Indices.size(), GL_UNSIGNED_INT, 0);
    }
};