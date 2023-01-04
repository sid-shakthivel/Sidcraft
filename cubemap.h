#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

// #include "shader.h"

namespace Skybox
{
    std::vector<const char *> ImagePaths = {
        "SkyBoxImages/right.png",
        "SkyBoxImages/left.png",
        "SkyBoxImages/top.png",
        "SkyBoxImages/bottom.png",
        "SkyBoxImages/back.png",
        "SkyBoxImages/front.png",
    };

    unsigned int VAO, VBO, EBO;
    unsigned int TextureId;

    float SkyboxVertices[] =
        {
            //   Coordinates
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f};

    unsigned int SkyboxIndices[] =
        {
            // Right
            1, 2, 6,
            6, 5, 1,
            // Left
            0, 4, 7,
            7, 3, 0,
            // Top
            4, 5, 6,
            6, 7, 4,
            // Bottom
            0, 3, 2,
            2, 1, 0,
            // Back
            0, 1, 5,
            5, 4, 0,
            // Front
            3, 7, 6,
            6, 2, 3};

    void
    Create()
    {
        // Setup rendering buffers
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
        glGenTextures(1, &TextureId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);

        int width, height, nrChannels;
        unsigned char *data;

        stbi_set_flip_vertically_on_load(false);

        for (unsigned int i = 0; i < ImagePaths.size(); i++)
        {
            data = stbi_load(ImagePaths[i], &width, &height, &nrChannels, 0);
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
    }

    void Draw(Shader *MeshShader)
    {
        glDepthFunc(GL_LEQUAL);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
        MeshShader->SetInt("Cubemap", 0);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthFunc(GL_LESS);
    }
}