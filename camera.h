// #include "matrix.h"

class Camera
{
private:
    Vector3f CameraPos;
    Vector3f CameraTarget;

public:
    Vector3f CameraFront = Vector3f(0.0f, 0.0f, -1.0f);
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