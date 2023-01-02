// #include "matrix.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
    Vector3f CameraPos;
    Vector3f CameraTarget;

public:
    Vector3f CameraFront = Vector3f(0.0f, 0.0f, 1.0f);
    Vector3f Up = Vector3f(0.0f, 1.0f, 0.0f);

    Camera(Vector3f cameraPos, Vector3f cameraTarget) : CameraPos(0.0f, 0.0f, 0.0f), CameraTarget(0.0f, 0.0f, 0.0f)
    {
        CameraPos = cameraPos;
        CameraTarget = cameraTarget;
    }

    Matrix4f RetrieveLookAt()
    {
        return CreateLookAtMatrix(CameraPos, CameraPos.Add(CameraFront), Up);
    }

    glm::mat4 TestLookAt()
    {
        // return CreateLookAtMatrix(CameraPos, CameraPos.Add(CameraFront), Up);
        glm::vec3 GCameraPos = glm::vec3(CameraPos.x, CameraPos.y, CameraPos.z);
        glm::vec3 GCameraFront = glm::vec3(CameraFront.x, CameraFront.y, CameraFront.z);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 CorrectLookAt = glm::lookAt(GCameraPos, GCameraPos + GCameraFront, cameraUp);
        return CorrectLookAt;
    }

    Matrix4f RetrieveSlimLookAtMatrix()
    {
        return CreateSlimLookAtMatrix(CameraPos, CameraPos.Add(CameraFront), Up);
    }

    Vector3f GetCameraPos()
    {
        return CameraPos;
    }

    void SetCameraPos(Vector3f cameraPos)
    {
        CameraPos = cameraPos;
    }

    Vector3f GetCameraTarget()
    {
        return CameraTarget;
    }
};