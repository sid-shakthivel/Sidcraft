#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/Matrix.h"
#include "../include/Chunk.h"
#include "../include/Camera.h"

const float GRAVITY = -50.0f;
const float JUMP_POWER = 30.0f;
static float UpwardsSpeed = 0.0f;

Camera *Camera::GetInstance(Vector3f cameraPos, Vector3f cameraTarget)
{
    if (Camera_ == nullptr)
        Camera_ = new Camera(cameraPos, cameraTarget);
    return Camera_;
}

Camera *Camera::GetInstance()
{
    if (Camera_ == nullptr)
    {
        std::cout << "Camera singleton must be defined";
        std::exit(0);
    }
    return Camera_;
}

Camera::Camera(Vector3f cameraPos, Vector3f cameraTarget) : CameraPos(0.0f, 0.0f, 0.0f), CameraTarget(0.0f, 0.0f, 0.0f)
{
    CameraPos = cameraPos;
    CameraTarget = cameraTarget;
}

Vector3f Camera::GetCameraPos()
{
    return CameraPos;
}

Matrix4f Camera::RetrieveLookAt()
{
    return CreateLookAtMatrix(CameraPos, CameraPos.Add(CameraFront), Up);
}

glm::vec3 Camera::TestValue()
{
    glm::vec3 GCameraPos = glm::vec3(CameraPos.x, CameraPos.y, CameraPos.z);
    glm::vec3 GCameraFront = glm::vec3(CameraFront.x, CameraFront.y, CameraFront.z);

    return GCameraPos;
}

glm::mat4 Camera::TestLookAt()
{
    glm::vec3 GCameraPos = glm::vec3(CameraPos.x, CameraPos.y, CameraPos.z);
    glm::vec3 GCameraFront = glm::vec3(CameraFront.x, CameraFront.y, CameraFront.z);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 CorrectLookAt = glm::lookAt(GCameraPos, GCameraPos + GCameraFront, cameraUp);
    return CorrectLookAt;
}

Matrix4f Camera::RetrieveSlimLookAtMatrix()
{
    return CreateSlimLookAtMatrix(CameraPos, CameraPos.Add(CameraFront), Up);
}

BlockType Camera::GetSelectedBlockType()
{
    return SelectedBlock;
}

void Camera::Move(GLFWwindow *window, float DeltaTime, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE])
{
    auto Height = Heightmap[(int)CameraPos.z][(int)CameraPos.x];

    if (CameraPos.z < 0 || CameraPos.x < 0)
        Height = 6;

    float CameraSpeed = DeltaTime * 10.0f;

    // Handle all inputs

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        CameraPos = CameraPos.Add(CameraFront.Multiply(CameraSpeed));
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        CameraPos = CameraPos.Sub(CameraFront.Multiply(CameraSpeed));
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        Vector3f Direction = CameraFront.CrossProduct(CameraFront, Up).ReturnNormalise();
        CameraPos = CameraPos.Sub(Direction.Multiply(CameraSpeed));
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        Vector3f Direction = CameraFront.CrossProduct(CameraFront, Up).ReturnNormalise();
        CameraPos = CameraPos.Add(Direction.Multiply(CameraSpeed));
    }
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        Jump();
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        CameraPos = CameraPos.Add(Up.ReturnNormalise().Multiply(CameraSpeed));
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        CameraPos = CameraPos.Sub(Up.ReturnNormalise().Multiply(CameraSpeed));
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        SelectedBlock = BlockType::Grass;
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        SelectedBlock = BlockType::Dirt;
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        SelectedBlock = BlockType::Stone;
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        SelectedBlock = BlockType::Sand;

    // UpwardsSpeed += GRAVITY * DeltaTime;
    // CameraPos.y += UpwardsSpeed * DeltaTime;

    // if (CameraPos.y < (Height + 5))
    // {
    //     UpwardsSpeed = 0;
    //     CameraPos.y = Height + 5;
    // }
};

void Camera::Jump()
{
    UpwardsSpeed = JUMP_POWER;
}

void Camera::Rotate(double XPos, double YPos)
{
    if (IsFirstMouse)
    {
        LastX = XPos;
        LastY = YPos;
        IsFirstMouse = false;
    }

    // Calculate offset
    float XOffset = XPos - LastX;
    float YOffset = LastY - YPos;
    LastX = XPos;
    LastY = YPos;

    float sensitivity = 0.1f;
    XOffset *= sensitivity;
    YOffset *= sensitivity;

    Yaw += XOffset;
    Pitch += YOffset;

    // Constrain the value
    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    float x = cos(ConvertToRadians(Yaw)) * cos(ConvertToRadians(Pitch));
    float y = sin(ConvertToRadians(Pitch));
    float z = sin(ConvertToRadians(Yaw)) * cos(ConvertToRadians(Pitch));

    CameraFront = Vector3f(x, 0, z).ReturnNormalise();
}

float Camera::ConvertToRadians(float Degrees)
{
    return Degrees * 3.14159 / 180;
}