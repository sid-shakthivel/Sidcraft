#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/Camera.h"
#include "../include/Shader.h"
#include "../include/World.h"
#include "../include/TextureAtlas.h"

#include "../include/Renderer.h"

Renderer::Renderer() : SlimViewMatrix(Camera::GetInstance()->RetrieveSlimLookAtMatrix())
{
    // Setup matrices and vectors
    SkyColour = Vector3f(0.5f, 0.5f, 0.5f);
    LightDir = glm::vec3(0.5f, 0.5f, 0.5f);
    LightPosition = glm::vec3(2.0f, 3.0f, -4.0f);
    CustomLightDir = Vector3f(2.0f, 3.0f, -4.0f);

    LightProjectionMatrix = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, 1.0f, 7.5f);
    LightViewMatrix = glm::lookAt(LightPosition, LightPosition + LightDir, glm::vec3(0.0, 1.0, 0.0));
    LightSpaceMatrix = LightProjectionMatrix * LightViewMatrix;

    ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.01f, 1000.0f);
    ViewMatrix = Camera::GetInstance()->TestLookAt();

    CameraViewPosition = Camera::GetInstance()->GetCameraPos();
    SlimViewMatrix = Camera::GetInstance()->RetrieveSlimLookAtMatrix();
}

void Renderer::RenderScene(Shader *GenericShader)
{
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GenericShader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureAtlas::GetInstance()->GetTextureAtlasId());

    GenericShader->SetInt("diffuseTexture", 0);

    GenericShader->SetVector3f("viewPos", &CameraViewPosition);
    GenericShader->SetVector3f("lightPos", &CustomLightDir);

    GenericShader->SetVector3f("SkyColour", &SkyColour);

    GenericShader->SetMatrix4f("lightSpaceMatrix", (const float *)(&LightSpaceMatrix));
    GenericShader->SetMatrix4f("view", (const float *)(&ViewMatrix));
    GenericShader->SetMatrix4f("projection", (const float *)(&ProjectionMatrix));

    DrawWorld(GenericShader);
}

void Renderer::RenderSkybox(Shader *GenericShader, float DeltaTime)
{
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GenericShader->Use();
    // GenericShader->SetMatrix4f("view", (const float *)(&SlimViewMatrix));
    GenericShader->SetMatrix4f("projection", (const float *)(&ProjectionMatrix));
    GenericShader->SetVector3f("FogColour", &SkyColour);

    World::GetInstance()->skybox.Draw(GenericShader, DeltaTime);
}

void Renderer::Update()
{
    CameraViewPosition = Camera::GetInstance()->GetCameraPos();
    ViewMatrix = Camera::GetInstance()->TestLookAt();
    SlimViewMatrix = Camera::GetInstance()->RetrieveSlimLookAtMatrix();
}

void Renderer::DrawWorld(Shader *GenericShader)
{
    for (int i = 0; i < World::GetInstance()->ChunkData.size(); i++)
        World::GetInstance()->ChunkData.at(i).Draw(GenericShader, false, World::GetInstance()->ChunkPositions.at(i));

    for (auto const &Tree : World::GetInstance()->TreeList)
        Tree.Draw(GenericShader, false);
}

void Renderer::SetupHDR()
{
    glGenFramebuffers(1, &HdrFBO);
    glGenTextures(2, &ColourBuffers[0]);
    glBindFramebuffer(GL_FRAMEBUFFER, HdrFBO);

    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, ColourBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, ColourBuffers[i], 0);
    }

    glGenRenderbuffers(1, &RboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, RboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, HdrFBO);
    unsigned int Attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, Attachments);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetupBloom()
{
    glGenFramebuffers(2, &PingPongFBO[0]);
    glGenTextures(2, &PingPongBuffers[0]);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, PingPongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, PingPongBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, PingPongBuffers[i], 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetupDepth()
{
    glGenFramebuffers(1, &DepthMapFBO);
    glGenTextures(1, &DepthMapTexture);
    glBindTexture(GL_TEXTURE_2D, DepthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::tuple<glm::mat4, glm::mat4> Renderer::GetMatrices()
{
    return std::make_tuple(ProjectionMatrix, ViewMatrix);
}