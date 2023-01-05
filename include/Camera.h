#pragma once

#include <vector>

class Camera
{
private:
    Vector3f CameraPos;
    Vector3f CameraTarget;

public:
    Vector3f CameraFront = Vector3f(0.0f, 0.0f, 1.0f);
    Vector3f Up = Vector3f(0.0f, 1.0f, 0.0f);

    Camera(Vector3f cameraPos, Vector3f cameraTarget);

    Matrix4f RetrieveLookAt();
    glm::mat4 TestLookAt();
    Vector3f GetCameraPos();
    Matrix4f RetrieveSlimLookAtMatrix();

    void Move(GLFWwindow *window, float DeltaTime, int (&Heightmap)[160][160]);
};