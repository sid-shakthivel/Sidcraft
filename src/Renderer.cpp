#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/Camera.h"
#include "../include/Shader.h"
#include "../include/World.h"
#include "../include/Quad.h"
#include "../include/TextureAtlas.h"

#include "../include/Renderer.h"

Renderer::Renderer()
{
    // Setup matrices and vectors
    SkyColour = Vector3f(0.5f, 0.5f, 0.5f);

    ReflectionPlane = Vector4f(0, 1, 0, -WATER_LEVEL);
    RefractionPlane = Vector4f(0, -1, 0, WATER_LEVEL + 5);

    CustomLightDir = Vector3f(2.0f, 3.0f, -4.0f);
    WaterLightDir = Vector3f(2.0f, 3.0f, 4.0f);
    LightPosition = Vector3f(0.0f, 40.0f, 0.0f);

    LightProjectionMatrix = CreateOrthographicProjectionMatrix(-120.0f, 120.0f, -120.0f, 120.0f, 0.1f, 500.0f);
    LightViewMatrix = CreateLookAtMatrix(LightPosition, Vector3f(240.0f, -30.0f, 240.0f), Vector3f(0.0f, 1.0f, 0.0f));
    LightSpaceMatrix = LightViewMatrix.Multiply(LightProjectionMatrix);

    ProjectionMatrix = CreatePerspectiveProjectionMatrix(Camera::ConvertToRadians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.01f, 1000.0f);
    ViewMatrix = Camera::GetInstance()->RetrieveLookAt();
    CameraViewPosition = Camera::GetInstance()->GetCameraPos();

    DuDvMap = LoadRBGFromFile("res/waterDUDV.png");
    WaterNormalMap = LoadRBGFromFile("res/WaterNormalMap.png");
}

void Renderer::RenderHDR(Shader *GenericShader, float RunningTime)
{
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, HdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderScene(GenericShader, RunningTime, false);
}

void Renderer::DrawTempQuad(Shader *GenericShader, Quad *FinalQuad)
{
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, WaterNormalMap);

    GenericShader->Use();
    GenericShader->SetInt("Image", 0);
    FinalQuad->Draw();
}

void Renderer::RenderBlur(Shader *BlurShader, Quad *FinalQuad)
{
    BlurShader->Use();

    Horizontal = true;
    bool FirstIteration = true;
    int Amount = 20;

    for (unsigned int i = 0; i < Amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, PingPongFBO[Horizontal]);

        BlurShader->SetInt("Horizontal", Horizontal);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(
            GL_TEXTURE_2D, FirstIteration ? ColourBuffers[1] : PingPongBuffers[!Horizontal]);

        BlurShader->SetInt("Image", 0);

        FinalQuad->Draw();

        Horizontal = !Horizontal;

        if (FirstIteration)
            FirstIteration = false;
    }
}

void Renderer::RenderBloom(Shader *BlendShader, Quad *FinalQuad)
{
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    BlendShader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ColourBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, PingPongBuffers[!Horizontal]);

    BlendShader->SetInt("Scene", 0);
    BlendShader->SetInt("BloomBlur", 1);
    BlendShader->SetFloat("Exposure", 0.10f);

    FinalQuad->Draw();
}

void Renderer::RenderSkybox(Shader *GenericShader, float DeltaTime)
{
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    DrawSkybox(GenericShader, DeltaTime);
}

void Renderer::DrawSkybox(Shader *GenericShader, float DeltaTime)
{
    GenericShader->Use();
    GenericShader->SetMatrix4f("Projection", (const float *)(&ProjectionMatrix));
    GenericShader->SetVector3f("FogColour", &SkyColour);

    World::GetInstance()->skybox.Draw(GenericShader, DeltaTime);
}

void Renderer::Update()
{
    CameraViewPosition = Camera::GetInstance()->GetCameraPos();
    ViewMatrix = Camera::GetInstance()->RetrieveLookAt();
}

void Renderer::RenderNormal(Shader *GenericShader, float RunningTime)
{
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderScene(GenericShader, RunningTime, false);
}

void Renderer::RenderScene(Shader *GenericShader, float RunningTime, bool IsDepth)
{
    GenericShader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureAtlas::GetInstance()->GetTextureAtlasId());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, DepthMapTexture);

    GenericShader->SetInt("MainTexture", 0);
    GenericShader->SetInt("ShadowMap", 1);

    GenericShader->SetVector3f("ViewPos", &CameraViewPosition);
    GenericShader->SetVector3f("LightDirection", &CustomLightDir);

    GenericShader->SetVector3f("SkyColour", &SkyColour);

    GenericShader->SetMatrix4f("LightSpaceMatrix", (const float *)(&LightSpaceMatrix));
    GenericShader->SetMatrix4f("View", (const float *)(&ViewMatrix));
    GenericShader->SetMatrix4f("Projection", (const float *)(&ProjectionMatrix));

    DrawWorld(GenericShader, RunningTime, IsDepth);
}

void Renderer::DrawWorld(Shader *GenericShader, float RunningTime, bool IsDepth)
{
    for (int i = 0; i < World::GetInstance()->ChunkData.size(); i++)
        World::GetInstance()->ChunkData.at(i).Draw(GenericShader, IsDepth, World::GetInstance()->ChunkPositions.at(i));

    for (auto const &Tree : World::GetInstance()->TreeList)
        Tree.Draw(GenericShader, IsDepth, RunningTime);

    for (int i = 0; i < World::GetInstance()->FlowerList.size(); i++)
        World::GetInstance()->FlowerList.at(i).Draw(GenericShader, IsDepth);

    // for (int i = 0; i < World::GetInstance()->LightCubes.size(); i++)
    //     World::GetInstance()->LightCubes.at(i).Draw(GenericShader, World::GetInstance()->LightCubePositions.at(i));
}

void Renderer::RenderReflection(Shader *GenericShader)
{
    glEnable(GL_CLIP_DISTANCE0);

    glViewport(0, 0, REFLECTION_WIDTH, REFLECTION_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, WaterReflectionFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GenericShader->Use();
    GenericShader->SetVector4f("HorizontalPlane", &ReflectionPlane);

    RenderScene(GenericShader, 1, false);
}

void Renderer::RenderRefraction(Shader *GenericShader)
{
    glEnable(GL_CLIP_DISTANCE0);

    glViewport(0, 0, REFRACTION_WIDTH, REFRACTION_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, WaterRefractionFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GenericShader->Use();
    GenericShader->SetVector4f("HorizontalPlane", &RefractionPlane);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureAtlas::GetInstance()->GetTextureAtlasId());

    GenericShader->SetInt("MainTexture", 0);

    GenericShader->SetMatrix4f("View", (const float *)(&ViewMatrix));
    GenericShader->SetMatrix4f("Projection", (const float *)(&ProjectionMatrix));

    DrawWorld(GenericShader, 1, false);
}

void Renderer::RenderWater(Shader *WaterShader, float DeltaTime)
{
    MoveFactor += DeltaTime * WAVE_SPEED;
    MoveFactor = MoveFactor >= 1 ? 0 : MoveFactor;

    WaterShader->Use();

    WaterShader->SetMatrix4f("View", (const float *)(&ViewMatrix));
    WaterShader->SetMatrix4f("Projection", (const float *)(&ProjectionMatrix));
    WaterShader->SetFloat("MoveFactor", MoveFactor);
    WaterShader->SetVector3f("CameraPos", &CameraViewPosition);
    WaterShader->SetVector3f("LightDirection", &WaterLightDir);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, WaterReflectionColour);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, WaterRefractionColour);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, TextureAtlas::GetInstance()->GetTextureAtlasId());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, DuDvMap);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, WaterNormalMap);

    WaterShader->SetInt("ReflectionTexture", 0);
    WaterShader->SetInt("RefractionTexture", 1);
    WaterShader->SetInt("MainTexture", 2);
    WaterShader->SetInt("DuDvMap", 3);
    WaterShader->SetInt("NormalMap", 4);

    for (int i = 0; i < World::GetInstance()->ChunkData.size(); i++)
        World::GetInstance()->ChunkData.at(i).DrawWater(WaterShader, World::GetInstance()->ChunkPositions.at(i));
}

void Renderer::RenderDepth(Shader *DepthShader)
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE0, DepthMapTexture);

    DepthShader->Use();

    DepthShader->SetMatrix4f("LightSpaceMatrix", (const float *)(&LightSpaceMatrix));

    DrawWorld(DepthShader, 1.0, true);
}

void Renderer::DrawDepthQuad(Shader *GenericShader, Quad *FinalQuad)
{
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, DepthMapTexture);

    GenericShader->Use();
    GenericShader->SetInt("Image", 0);
    FinalQuad->Draw();
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

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: INCOMPLETE DEPTH FRAMEBUFFER" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetupHDR()
{
    // Create 2 floating point colour buffers (1 for normal rendering, 1 for brightness threshold values)
    glGenFramebuffers(1, &HdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, HdrFBO);
    glGenTextures(2, ColourBuffers);

    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, ColourBuffers[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, ColourBuffers[i], 0);
    }

    // Create and attach depth buffer
    glGenRenderbuffers(1, &RboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, RboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RboDepth);

    // Determine which colour attachements are used
    unsigned int Attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, Attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: INCOMPLETE HDR FRAMEBUFFER" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetupReflection()
{
    glGenFramebuffers(1, &WaterReflectionFBO);
    glGenTextures(1, &WaterReflectionColour);

    glBindFramebuffer(GL_FRAMEBUFFER, WaterReflectionFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glBindTexture(GL_TEXTURE_2D, WaterReflectionColour);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                 REFLECTION_WIDTH, REFLECTION_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, WaterReflectionColour, 0);

    glGenRenderbuffers(1, &WaterReflectionRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, WaterReflectionRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, REFLECTION_WIDTH, REFLECTION_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, WaterReflectionRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetupRefraction()
{
    glGenFramebuffers(1, &WaterRefractionFBO);
    glGenTextures(1, &WaterRefractionColour);

    glBindFramebuffer(GL_FRAMEBUFFER, WaterRefractionFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glBindTexture(GL_TEXTURE_2D, WaterRefractionColour);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                 REFRACTION_WIDTH, REFRACTION_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, WaterRefractionColour, 0);

    glGenTextures(1, &WaterRefractionDepth);
    glBindTexture(GL_TEXTURE_2D, WaterRefractionDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 REFRACTION_WIDTH, REFRACTION_WIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, WaterRefractionDepth, 0);
}

void Renderer::SetupBloom()
{
    glGenFramebuffers(2, PingPongFBO);
    glGenTextures(2, PingPongBuffers);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, PingPongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, PingPongBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PingPongBuffers[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR: INCOMPLETE PINGPONG FRAMEBUFFER" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetupGBuffer()
{
    glGenFramebuffers(1, &GFB);
    glBindFramebuffer(GL_FRAMEBUFFER, GFB);

    glGenTextures(1, &GPosition);
    glBindTexture(GL_TEXTURE_2D, GPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GPosition, 0);

    glGenTextures(1, &GNormals);
    glBindTexture(GL_TEXTURE_2D, GNormals);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, GNormals, 0);

    glGenTextures(1, &GColourSpec);
    glBindTexture(GL_TEXTURE_2D, GColourSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, GColourSpec, 0);

    // Create and attach depth buffer
    glGenRenderbuffers(1, &GRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, GRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, GRBO);

    unsigned int Attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, Attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: INCOMPLETE HDR FRAMEBUFFER" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}