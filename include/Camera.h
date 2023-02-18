#pragma once

#include <vector>
#include "Matrix.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
    Vector3f CameraPos;
    Vector3f CameraTarget;

    bool IsFirstMouse = true;
    float LastX = 400;
    float LastY = 300;
    float Yaw = 90.0f;
    float Pitch = 0;

    float ConvertToRadians(float Degrees);
    void Jump();

protected:
    Camera(Vector3f cameraPos, Vector3f cameraTarget);
    static Camera *Camera_;

public:
    Vector3f CameraFront = Vector3f(0.0f, 0.0f, 1.0f);
    Vector3f Up = Vector3f(0.0f, 1.0f, 0.0f);

    Camera(Camera &other) = delete;
    void operator=(const Camera &) = delete;

    static Camera *GetInstance(Vector3f cameraPos, Vector3f cameraTarget);
    static Camera *GetInstance();

    Matrix4f RetrieveLookAt();
    glm::mat4 TestLookAt();
    Vector3f GetCameraPos();
    glm::vec3 TestValue();
    Matrix4f RetrieveSlimLookAtMatrix();

    void Move(GLFWwindow *window, float DeltaTime, int (&Heightmap)[240][240]);
    void Rotate(double XPos, double YPos);
};

inline Camera *Camera::Camera_ = nullptr;