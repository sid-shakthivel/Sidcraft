#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "../include/stb_image.h"

#include "../include/Shader.h"

#include "../include/Skybox.h"

Skybox::Skybox() : ModelMatrix(Matrix4f(1.0f))
{
    ModelMatrix = Matrix4f(1.0f);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertices), &SkyboxVertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SkyboxIndices), &SkyboxIndices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // Setup position attribute
    glEnableVertexAttribArray(0);

    // Sort out textures
    glGenTextures(1, &LightTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, LightTextureId);

    int width, height, nrChannels;
    unsigned char *data;

    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < LightImagePaths.size(); i++)
    {
        data = stbi_load(LightImagePaths[i], &width, &height, &nrChannels, 0);
        if (!data)
            std::cout << "ERROR LOADING TEXTURE" << std::endl;
        else
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &DarkTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, DarkTextureId);

    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < DarkImagePaths.size(); i++)
    {
        data = stbi_load(DarkImagePaths[i], &width, &height, &nrChannels, 0);
        if (!data)
            std::cout << "ERROR LOADING TEXTURE 2" << std::endl;
        else
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::Draw(Shader *MeshShader, float DeltaTime)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, LightTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, DarkTextureId);

    MeshShader->SetInt("Cubemap", 0);
    MeshShader->SetInt("Cubemap2", 1);

    UpdateBlend(DeltaTime, MeshShader);

    RotationAngle += 3.0f * DeltaTime;
    auto AnglesInRadians = RotationAngle * 3.14159 / 180;

    ModelMatrix.Rotate(AnglesInRadians, Y_AXIS);

    glDepthFunc(GL_LEQUAL);

    glBindVertexArray(VAO);

    MeshShader->SetMatrix4f("Model", (const float *)(&ModelMatrix));

    MeshShader->SetFloat("BlendFactor", BlendFactor);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDepthFunc(GL_LESS);
}

float Lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void Skybox::UpdateBlend(float DeltaTime, Shader *MeshShader)
{
    TotalTime += DeltaTime;

    if (TotalTime > 40)
        TotalTime = 0;
    else if (TotalTime > 25)
        BlendFactor = 1.0f - Lerp(0.0f, 1.0f, (TotalTime - 20) * (1.0f / 20.0f));
    else if (TotalTime > 20)
        BlendFactor = 1.0f;
    else if (TotalTime > 5)
        BlendFactor = Lerp(0.0f, 1.0f, TotalTime * (1.0f / 20.0f));
    else
        BlendFactor = 0.0f;
}