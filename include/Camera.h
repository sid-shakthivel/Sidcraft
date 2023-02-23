#pragma once

#include <vector>
#include "Matrix.h"
#include "Block.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    BlockType SelectedBlock = BlockType::Stone;

protected:
    Camera(Vector3f cameraPos, Vector3f cameraTarget);
    static Camera *Camera_;

public:
    Vector3f CameraPos;
    Vector3f CameraFront = Vector3f(0.0f, 0.0f, 1.0f);
    Vector3f Up = Vector3f(0.0f, 1.0f, 0.0f);

    Camera(Camera &other) = delete;
    void operator=(const Camera &) = delete;

    static Camera *GetInstance(Vector3f cameraPos, Vector3f cameraTarget);
    static Camera *GetInstance();

    Matrix4f RetrieveLookAt();
    Vector3f GetCameraPos();

    void InvertPitch();

    BlockType GetSelectedBlockType();

    static float ConvertToRadians(float Degrees);

    void Move(GLFWwindow *window, float DeltaTime, int (&Heightmap)[WORLD_SIZE][WORLD_SIZE]);
    void Rotate(double XPos, double YPos);
};

inline Camera *Camera::Camera_ = nullptr;