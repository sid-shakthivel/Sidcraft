#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/Matrix.h"
#include "../include/Chunk.h"
#include "../include/Camera.h"

const float GRAVITY = -50.0f;
const float JUMP_POWER = 30.0f;
static float UpwardsSpeed = 0.0f;

Camera::Camera(Vector3f cameraPos, Vector3f cameraTarget)
{
    CameraPos = cameraPos;
    CameraTarget = cameraTarget;
}

Vector3f Camera::GetCameraPos()
{
    return CameraPos;
}

void Camera::SetCameraPos(Vector3f Pos)
{
    CameraPos = Pos;
}

Matrix4f Camera::RetrieveLookAt()
{
    return CreateLookAtMatrix(CameraPos, CameraPos.Add(CameraFront), Up);
}

void Camera::Jump()
{
    UpwardsSpeed = JUMP_POWER;
}

void Camera::InvertPitch()
{
    Pitch = -Pitch;
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
    float y = 0;
    float z = sin(ConvertToRadians(Yaw)) * cos(ConvertToRadians(Pitch));

    CameraFront = Vector3f(x, y, z).ReturnNormalise();
}

float Camera::ConvertToRadians(float Degrees)
{
    return Degrees * 3.14159 / 180;
}