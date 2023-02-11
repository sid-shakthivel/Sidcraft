#pragma once

#include "Matrix.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MouseHandler
{
private:
    Vector3f ConvertToNDC(double ViewportXPos, double ViewportYPos);
    Vector4f ConvertToClipSpace(Vector3f NDCVec);
    Vector4f ConvertToCameraCoords(Vector4f ClipVec, glm::mat4 ProjectionMatrix);
    Vector3f ConvertToWorld(Vector4f CameraVec, glm::mat4 ViewMatrix);

public:
    MouseHandler(){};
    Vector3f GetRay(double ViewportXPos, double ViewportYPos, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix);
};

void MouseCallback(GLFWwindow *window, double xpos, double ypos);
// void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);