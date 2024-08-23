#pragma once

#include <vector>
#include "Matrix.h"
#include "Block.h"

class Camera
{
private:
    Vector3f CameraTarget;

    bool IsFirstMouse = true;
    float LastX = 400;
    float LastY = 300;
    float Yaw = 90.0f;
    float Pitch = 0;

    void Jump();

public:
    Vector3f CameraPos;
    Vector3f CameraFront = Vector3f(0.0f, 0.0f, 1.0f);
    Vector3f Up = Vector3f(0.0f, 1.0f, 0.0f);

    Camera(Vector3f cameraPos, Vector3f cameraTarget);

    Matrix4f RetrieveLookAt();
    Vector3f GetCameraPos();
    void SetCameraPos(Vector3f Pos);
    void InvertPitch();
    static float ConvertToRadians(float Degrees);
    void Rotate(double XPos, double YPos);
};
