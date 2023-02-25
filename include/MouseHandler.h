#pragma once

#include "Matrix.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MouseHandler
{
private:
    glm::mat4 ProjectionMatrix;
    glm::vec3 UpVec = glm::vec3(0.0f, 1.0f, 0.0f);

    Vector3f ConvertToNDC(double ViewportXPos, double ViewportYPos);
    Vector4f ConvertToClipSpace(Vector3f NDCVec);
    Vector4f ConvertToCameraCoords(Vector4f ClipVec);
    Vector3f ConvertToWorld(Vector4f CameraVec, Vector3f CameraPos, Vector3f CameraFront);

public:
    MouseHandler();
    Vector3f GetRay(double ViewportXPos, double ViewportYPos, Vector3f CameraPos, Vector3f CameraFront);
};

void MouseCallback(GLFWwindow *window, double xpos, double ypos);
// void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);