#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/Matrix.h"
#include "../include/Mesh.h"
#include "../include/Camera.h"

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

void Camera::Move(GLFWwindow *window, float DeltaTime, int (&Heightmap)[160][160])
{
    float CameraSpeed = DeltaTime * 64.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        CameraPos = CameraPos.Add(CameraFront.Multiply(CameraSpeed));
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        CameraPos = CameraPos.Sub(CameraFront.Multiply(CameraSpeed));
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        Vector3f Direction = CameraFront.CrossProduct(CameraFront, Up).ReturnNormalise();
        CameraPos = CameraPos.Sub(Direction.Multiply(CameraSpeed));
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        Vector3f Direction = CameraFront.CrossProduct(CameraFront, Up).ReturnNormalise();
        CameraPos = CameraPos.Add(Direction.Multiply(CameraSpeed));

        // auto Height = Heightmap[(int)CameraPos.z][(int)CameraPos.x];
        // if (CameraPos.y < Height)
        //     CameraPos.y = Height + 5;
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        Vector3f Direction = Up.ReturnNormalise();
        CameraPos = CameraPos.Add(Direction.Multiply(CameraSpeed));
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        Vector3f Direction = Up.ReturnNormalise();
        CameraPos = CameraPos.Sub(Direction.Multiply(CameraSpeed));

        // Get height of position in which we are

        if (CameraPos.z > 0 && CameraPos.x > 0)
        {
            auto Height = Heightmap[(int)CameraPos.z][(int)CameraPos.x];
            if (CameraPos.y < Height)
                CameraPos.y = Height + 5;
        }
    }
};