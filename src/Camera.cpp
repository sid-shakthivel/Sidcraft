#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/Vector.h"
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
    return CreateLookAtMatrix(CameraPos, CameraPos + CameraFront, Up);
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

    CameraFront = Vector3f(x, y, z).Normalise();
}

float Camera::ConvertToRadians(float Degrees)
{
    return Degrees * 3.14159 / 180;
}

Matrix4f Camera::CreatePerspectiveProjectionMatrix(float Fov, float Aspect, float Near, float Far)
{
    Matrix4f ProjectionMatrix = Matrix4f(0);

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            ProjectionMatrix.elements[i][j] = 0;

    ProjectionMatrix.elements[0][0] = 1.0f / tan(Fov / 2.0f) / Aspect;
    ProjectionMatrix.elements[1][1] = 1.0f / tan(Fov / 2.0f);
    ProjectionMatrix.elements[2][2] = (Far + Near) / (Near - Far);
    ProjectionMatrix.elements[2][3] = -1.0f;
    ProjectionMatrix.elements[3][2] = (2 * Far * Near) / (Near - Far);

    return ProjectionMatrix;
}

Matrix4f Camera::CreateOrthographicProjectionMatrix(float Bottom, float Top, float Left, float Right, float Near, float Far)
{
    Matrix4f MatrixA = Matrix4f(1);

    MatrixA.elements[0][0] = 2 / (Right - Left);
    MatrixA.elements[0][1] = 0;
    MatrixA.elements[0][2] = 0;
    MatrixA.elements[0][3] = 0;

    MatrixA.elements[1][0] = 0;
    MatrixA.elements[1][1] = 2 / (Top - Bottom);
    MatrixA.elements[1][2] = 0;
    MatrixA.elements[1][3] = 0;

    MatrixA.elements[2][0] = 0;
    MatrixA.elements[2][1] = 0;
    MatrixA.elements[2][2] = -2 / (Far - Near);
    MatrixA.elements[2][3] = 0;

    MatrixA.elements[3][0] = -(Right + Left) / (Right - Left);
    MatrixA.elements[3][1] = -(Top + Bottom) / (Top - Bottom);
    MatrixA.elements[3][2] = -(Far + Near) / (Far - Near);
    MatrixA.elements[3][3] = 1;

    return MatrixA;
}

Matrix4f Camera::CreateLookAtMatrix(Vector3f PositionVector, Vector3f TargetVector, Vector3f UpVector)
{
    Matrix4f MatrixA = Matrix4f(1);

    Vector3f ZAxis = (TargetVector - PositionVector).Normalise();
    Vector3f XAxis = ZAxis.CrossProduct(UpVector).Normalise();
    Vector3f YAxis = XAxis.CrossProduct(ZAxis);

    ZAxis.Negate();

    MatrixA.elements[0][0] = XAxis.x;
    MatrixA.elements[0][1] = XAxis.y;
    MatrixA.elements[0][2] = XAxis.z;
    MatrixA.elements[0][3] = -(XAxis.DotProduct(PositionVector));

    MatrixA.elements[1][0] = YAxis.x;
    MatrixA.elements[1][1] = YAxis.y;
    MatrixA.elements[1][2] = YAxis.z;
    MatrixA.elements[1][3] = -(YAxis.DotProduct(PositionVector));

    MatrixA.elements[2][0] = ZAxis.x;
    MatrixA.elements[2][1] = ZAxis.y;
    MatrixA.elements[2][2] = ZAxis.z;
    MatrixA.elements[2][3] = -(ZAxis.DotProduct(PositionVector));

    MatrixA.elements[3][0] = 0;
    MatrixA.elements[3][1] = 0;
    MatrixA.elements[3][2] = 0;
    MatrixA.elements[3][3] = 1;

    MatrixA.ConvertToColumnMajorOrder();

    return MatrixA;
}