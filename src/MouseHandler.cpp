#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/Matrix.h"
#include "../include/World.h"
#include "../include/Renderer.h"
#include "../include/Camera.h"

#include "../include/MouseHandler.h"

void MouseHandler::Initialise()
{
    ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.01f, 1000.0f);
}

// Convert Viewport coordinates into NDC of range (-1-1)
Vector3f MouseHandler::ConvertToNDC(double ViewportXPos, double ViewportYPos)
{
    float X = (2.0f * ViewportXPos) / WINDOW_WIDTH - 1.0f;
    float Y = 1.0f - (2.0f * ViewportYPos) / WINDOW_HEIGHT;
    float Z = 1.0f;

    return Vector3f(X, Y, Z);
}

// Point ray forwards (negative z direction)
Vector4f MouseHandler::ConvertToClipSpace(Vector3f NDCVec)
{
    return Vector4f(NDCVec.x, NDCVec.y, -1.0f, 1.0f);
}

// Multiply by inverse of projection matrix to unproject it
Vector4f MouseHandler::ConvertToCameraCoords(Vector4f ClipVec)
{
    glm::vec4 CameraRay = inverse(ProjectionMatrix) * glm::vec4(ClipVec.x, ClipVec.y, ClipVec.z, ClipVec.w);
    return Vector4f(CameraRay.x, CameraRay.y, -1.0f, 0.0f);
}

Vector3f MouseHandler::ConvertToWorld(Vector4f CameraVec, Vector3f CameraPos, Vector3f CameraFront)
{
    glm::vec3 GLMCameraPos = glm::vec3(CameraPos.x, CameraPos.y, CameraPos.z);
    glm::mat4 ViewMatrix = glm::lookAt(GLMCameraPos, GLMCameraPos + glm::vec3(CameraFront.x, CameraFront.y, CameraFront.z), UpVec);
    glm::vec4 WorldRay = inverse(ViewMatrix) * glm::vec4(CameraVec.x, CameraVec.y, CameraVec.z, CameraVec.w);
    return Vector3f(WorldRay.x, WorldRay.y, WorldRay.z).ReturnNormalise();
}

// Attempts to return a ray near which user clicks
Vector3f MouseHandler::GetRay(double ViewportXPos, double ViewportYPos, Vector3f CameraPos, Vector3f CameraFront)
{
    Vector3f NDCCoords = ConvertToNDC(ViewportXPos, ViewportYPos);
    Vector4f ClipSpace = ConvertToClipSpace(NDCCoords);
    Vector4f CameraCoords = ConvertToCameraCoords(ClipSpace);
    Vector3f WorldCoords = ConvertToWorld(CameraCoords, CameraPos, CameraFront);

    return WorldCoords;
}

void MouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    // Camera::GetInstance()->Rotate(xpos, ypos);
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    // double XPos, YPos;
    // glfwGetCursorPos(window, &XPos, &YPos);

    // Vector3f Ray = MainMouseHandler.GetRay(XPos, YPos, Camera::GetInstance()->CameraPos, Camera::GetInstance()->CameraFront);

    // Vector3f PositionToTest;

    // bool IsFound = false;

    // for (int i = 1; i <= 16; i++)
    // {
    //     PositionToTest = Ray.Multiply(i).Add(Camera::GetInstance()->GetCameraPos().Add(Camera::GetInstance()->CameraFront));

    //     PositionToTest.x = std::min<float>(PositionToTest.x, WORLD_SIZE - 1);
    //     PositionToTest.z = std::min<float>(PositionToTest.z, WORLD_SIZE - 1);

    //     PositionToTest.x = std::max<float>(PositionToTest.x, 0);
    //     PositionToTest.z = std::max<float>(PositionToTest.z, 0);

    //     for (int Index = 0; Index < World::GetInstance()->ChunkData.size(); Index++)
    //     {
    //         auto TempChunk = &World::GetInstance()->ChunkData.at(Index);

    //         if (TempChunk->IsWithinChunk(PositionToTest))
    //         {
    //             auto NewChunk = Chunk(TempChunk->Blocks);

    //             if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    //                 NewChunk.SetChunk(PositionToTest.Sub(Ray), Offset, World::GetInstance()->Heightmap);
    //             else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    //                 NewChunk.ClearChunk(PositionToTest, Offset);

    //             World::GetInstance()->ChunkData.erase(World::GetInstance()->ChunkData.begin() + Index);
    //             World::GetInstance()->ChunkData.push_back(NewChunk);

    //             IsFound = true;
    //             break;
    //         }
    //     }

    //     if (IsFound)
    //         break;
    // }
}

MouseHandler MainMouseHandler = MouseHandler();