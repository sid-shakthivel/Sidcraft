#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/Quad.h"

Quad::Quad(Vector3f ScaleFactor)
{
    ModelMatrix.Scale(ScaleFactor);
    CreateMesh();
}

void Quad::CreateMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(NewQuadData), &NewQuadData, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &this->Indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Quad::Draw(Shader *QuadShader)
{
    glBindVertexArray(VAO);
    QuadShader->SetMatrix4f("Model", (const float *)(&ModelMatrix));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(0 * sizeof(GLuint)));
}
