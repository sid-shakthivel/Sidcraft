#pragma once

#include "Matrix.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Renderer
{
private:
    Vector3f SkyColour;
    glm::vec3 LightDir;
    Vector3f CustomLightDir;
    glm::vec3 LightPosition;

    Vector3f CameraViewPosition;
    Matrix4f SlimViewMatrix;

    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;

    glm::mat4 LightProjectionMatrix;
    glm::mat4 LightViewMatrix;
    glm::mat4 LightSpaceMatrix;

    void DrawWorld(Shader *GenericShader);

protected:
public:
    Renderer();

    void RenderScene(Shader *GenericShader);
    void RenderSkybox(Shader *GenericShader, float DeltaTime);
    void Update();
};

const unsigned int SCREEN_WIDTH = 800 * 4;
const unsigned int SCREEN_HEIGHT = 600 * 4;