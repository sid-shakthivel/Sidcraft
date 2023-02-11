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

std::tuple<glm::mat4, glm::mat4> Renderer::GetMatrices()
{
    return std::make_tuple(ProjectionMatrix, ViewMatrix);
}