#pragma once

#include "Matrix.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tuple>
#include "Shader.h"
#include "Quad.h"

const float WAVE_SPEED = 0.03f;

class Renderer
{
private:
    // Matrices
    Matrix4f ViewMatrix = Matrix4f(1);
    Matrix4f ProjectionMatrix = Matrix4f(1);

    // Clipping Planes
    Vector4f ReflectionPlane;
    Vector4f RefractionPlane;

    // Textures
    unsigned int TitleTexture;
    unsigned int DepthMapTexture;
    unsigned int LightboxSkyboxTexture = 0;
    unsigned int DarkSkyboxTexture = 0;
    unsigned int ColourBuffers[2];
    unsigned int PingPongBuffers[2];
    unsigned int WaterReflectionColour;
    unsigned int WaterRefractionDepth;
    unsigned int WaterRefractionColour;
    unsigned int DuDvMap = 0;
    unsigned int WaterNormalMap = 0;

    // Render Buffers
    unsigned int RboDepth;
    unsigned int WaterReflectionRBO;

    // FBO's
    unsigned int HdrFBO;
    unsigned int PingPongFBO[2];
    unsigned int DepthMapFBO;
    unsigned int WaterReflectionFBO;
    unsigned int WaterRefractionFBO;

    // Other
    Vector3f SkyColour;
    Vector3f LightDir;
    Vector3f CameraViewPosition;
    unsigned int MatricesUBO;
    float MoveFactor = 0;
    bool Horizontal = true;

    std::vector<float> ShadowCascadeLevels{500.0f / 10.0f, 500.0f / 8.0f, 500.0f / 5.0f, 500.0 / 2.0f};

    void DrawWorld(Shader *GenericShader, float RunningTime, bool IsDepth);
    std::vector<Matrix4f> GetLightSpaceMatrices();

protected:
    void RenderScene(Shader *GenericShader, float RunningTime, bool IsDepth);

public:
    Renderer();
    ~Renderer();

    void SetupHDR();
    void SetupBloom();
    void SetupDepth();
    void SetupRefraction();
    void SetupReflection();
    void SetupMatrixUBO();
    void SetupTextures();

    void RenderBlur(Shader *BlurShader, Quad *FinalQuad);
    void RenderBloom(Shader *BlendShader, Quad *FinalQuad);
    void RenderRefraction(Shader *GenericShader);
    void RenderReflection(Shader *GenericShader);
    void RenderHDR(Shader *GenericShader, float RunningTime);
    void RenderNormal(Shader *GenericShader, float RunningTime);
    void RenderDepth(Shader *DepthShader);
    void RenderSkybox(Shader *GenericShader, float DeltaTime); // Specifically sets the framebuffer
    void DrawSkybox(Shader *GenericShader, float DeltaTime);   // Draws skybox to whatever framebuffer is set
    void RenderWater(Shader *WaterShader, float DeltaTime);

    void DrawDepthQuad(Shader *GenericShader, Quad *FinalQuad, int CurrentLayer);
    void DrawTempQuad(Shader *GenericShader, Quad *FinalQuad);

    void UBOPass();

    void Update();
};

extern unsigned int SCREEN_WIDTH;
extern unsigned int SCREEN_HEIGHT;

// static unsigned int SCREEN_WIDTH = 800;
// static unsigned int SCREEN_HEIGHT = 600;

// static unsigned int SCREEN_WIDTH = 2880;
// static unsigned int SCREEN_HEIGHT = 1694;

const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 4096;

const unsigned int REFLECTION_WIDTH = 320;
const unsigned int REFLECTION_HEIGHT = 180;

const unsigned int REFRACTION_WIDTH = 1280;
const unsigned int REFRACTION_HEIGHT = 720;

extern unsigned int WINDOW_WIDTH;
extern unsigned int WINDOW_HEIGHT;