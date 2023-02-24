#pragma once

#include "Matrix.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tuple>
#include "Shader.h"
#include "Quad.h"

const float WAVE_SPEED = 0.03f;

class Renderer
{
private:
    Vector3f SkyColour;

    Vector3f CustomLightDir;
    Vector3f LightPosition;

    Vector3f CameraViewPosition;

    Matrix4f ViewMatrix = Matrix4f(1);
    Matrix4f ProjectionMatrix = Matrix4f(1);
    Matrix4f TestViewMatrix = Matrix4f(1);

    Matrix4f LightProjectionMatrix = Matrix4f(1);
    Matrix4f LightViewMatrix = Matrix4f(1);
    Matrix4f LightSpaceMatrix = Matrix4f(1);

    Vector4f ReflectionPlane;
    Vector4f RefractionPlane;

    unsigned int HdrFBO;
    unsigned int PingPongFBO[2];
    unsigned int DepthMapFBO;
    unsigned int WaterReflectionFBO;
    unsigned int WaterRefractionFBO;

    unsigned int ColourBuffers[2];
    unsigned int PingPongBuffers[2];
    unsigned int Attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

    unsigned int DepthMapTexture;
    unsigned int WaterReflectionColour;
    unsigned int WaterRefractionDepth;
    unsigned int WaterRefractionColour;

    unsigned int RboDepth;
    unsigned int WaterReflectionRBO;

    unsigned int DuDvMap = 0;
    unsigned int WaterNormalMap = 0;
    float MoveFactor = 0;

    bool Horizontal = true;

    void DrawWorld(Shader *GenericShader, float DeltaTime, bool IsDepth);

protected:
    void RenderScene(Shader *GenericShader, float DeltaTime, bool IsDepth);

public:
    Renderer();

    void SetupHDR();
    void SetupBloom();
    void SetupDepth();
    void SetupRefraction();
    void SetupReflection();

    void RenderHDR(Shader *GenericShader, float DeltaTime);
    void RenderBlur(Shader *BlurShader, Quad *FinalQuad);
    void RenderBloom(Shader *BlendShader, Quad *FinalQuad);
    void RenderNormal(Shader *GenericShader, float DeltaTime);
    void RenderDepth(Shader *DepthShader, float DeltaTime);
    void RenderSkybox(Shader *GenericShader, float DeltaTime); // Specifically sets the framebuffer
    void DrawSkybox(Shader *GenericShader, float DeltaTime);   // Draws skybox to whatever framebuffer is set
    void RenderRefraction(Shader *GenericShader);
    void RenderReflection(Shader *GenericShader);
    void RenderWater(Shader *WaterShader, float RunningTime);

    void DrawDepthQuad(Shader *GenericShader, Quad *FinalQuad);
    void DrawTempQuad(Shader *GenericShader, Quad *FinalQuad);

    void Update();

    std::tuple<Matrix4f, Matrix4f> GetMatrices();
};

static unsigned int SCREEN_WIDTH = 800;
static unsigned int SCREEN_HEIGHT = 600;
// static unsigned int SCREEN_WIDTH = 800 * 4;
// static unsigned int SCREEN_HEIGHT = 600 * 4;
const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

const unsigned int REFLECTION_WIDTH = 320;
const unsigned int REFLECTION_HEIGHT = 180;

const unsigned int REFRACTION_WIDTH = 1280;
const unsigned int REFRACTION_HEIGHT = 720;