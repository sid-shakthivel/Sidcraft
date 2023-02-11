#include <iostream>
#include <glm/glm.hpp>
#include <string>
#include <glm/gtc/matrix_transform.hpp>

#include "../include/Matrix.h"
#include "../include/Renderer.h"

#include "../include/MouseHandler.h"

// Convert Viewport coordinates into NDC of range (-1-1)
Vector3f MouseHandler::ConvertToNDC(double ViewportXPos, double ViewportYPos)
{
    float X = (2.0f * ViewportXPos) / SCREEN_WIDTH - 1.0f;
    float Y = 1.0f - (2.0f * ViewportYPos) / SCREEN_HEIGHT;
    float Z = 1.0f;

    return Vector3f(X, Y, Z);
}

// Point ray forwards (negative z direction)
Vector4f MouseHandler::ConvertToClipSpace(Vector3f NDCVec)
{
    return Vector4f(NDCVec.x, NDCVec.y, -1.0f, 1.0f);
}

// Multiply by inverse of projection matrix to unproject it
Vector4f MouseHandler::ConvertToCameraCoords(Vector4f ClipVec, glm::mat4 ProjectionMatrix)
{
    glm::vec4 CameraRay = inverse(ProjectionMatrix) * glm::vec4(ClipVec.x, ClipVec.y, ClipVec.z, ClipVec.w);
    return Vector4f(CameraRay.x, CameraRay.y, -1.0f, 0.0f);
}

Vector3f MouseHandler::ConvertToWorld(Vector4f CameraVec, glm::mat4 ViewMatrix)
{
    glm::vec4 WorldRay = inverse(ViewMatrix) * glm::vec4(CameraVec.x, CameraVec.y, CameraVec.z, CameraVec.w);
    return Vector3f(WorldRay.x, WorldRay.y, WorldRay.z).ReturnNormalise();
}

// Attempts to return a ray near which user clicks
Vector3f MouseHandler::GetRay(double ViewportXPos, double ViewportYPos, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix)
{
    Vector3f NDCCoords = ConvertToNDC(ViewportXPos, ViewportYPos);
    Vector4f ClipSpace = ConvertToClipSpace(NDCCoords);
    Vector4f CameraCoords = ConvertToCameraCoords(ClipSpace, ProjectionMatrix);
    Vector3f WorldCoords = ConvertToWorld(CameraCoords, ViewMatrix);

    return WorldCoords;
}
