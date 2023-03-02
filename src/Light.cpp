#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// #include <glm/gtx/string_cast.hpp>

#include "../include/Renderer.h"
#include "../include/Light.h"

/*
    Get fustrum corners in world space by multiplying the inverse of the projection and view matrix to NDC corners
    NDC coordinates are in range -1 to 1
*/
std::vector<Vector4f> GetFustrumCornersWorldSpace(Vector3f CameraPos, Vector3f CameraFront, float Near, float Far)
{
    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, Near, Far);

    glm::vec3 GLMCameraPos = glm::vec3(CameraPos.x, CameraPos.y, CameraPos.z);
    glm::mat4 ViewMatrix = glm::lookAt(GLMCameraPos, GLMCameraPos + glm::vec3(CameraFront.x, CameraFront.y, CameraFront.z), glm::vec3(0.0f, 1.0f, 0.0f));

    const auto inv = glm::inverse(ProjectionMatrix * ViewMatrix);

    std::vector<Vector4f> FustrumCorners;
    for (unsigned int x = 0; x < 2; ++x)
        for (unsigned int y = 0; y < 2; ++y)
            for (unsigned int z = 0; z < 2; ++z)
            {
                glm::vec4 pt =
                    inv * glm::vec4(
                              2.0f * x - 1.0f,
                              2.0f * y - 1.0f,
                              2.0f * z - 1.0f,
                              1.0f);
                pt = pt / pt.w;

                FustrumCorners.push_back(Vector4f(pt.x, pt.y, pt.z, pt.w));
            }

    return FustrumCorners;
}

Matrix4f CalculateLightViewMatrix(std::vector<Vector4f> Corners, Vector3f LightDirection)
{
    /*
        Calculate centre by averaging out the 4 corners
        Fustrum has a near and far plane so by averaging those values, centre can be found
    */
    // glm::vec3 TempCentre = glm::vec3(0, 0, 0);
    // for (const auto Vertex : Corners)
    //     TempCentre += glm::vec3(Vertex.x, Vertex.y, Vertex.z);
    // TempCentre /= Corners.size();
    // Vector3f Centre = Vector3f(TempCentre.x, TempCentre.y, TempCentre.z);

    Vector3f Centre = Vector3f(0.0f, 0.0f, 0.0f);
    for (const auto Vertex : Corners)
        Centre = Centre.Add(Vector3f(Vertex.x, Vertex.y, Vertex.z));
    Centre = Centre.Divide(Corners.size());

    return CreateLookAtMatrix(Centre.Add(LightDirection), Centre, Vector3f(0.0f, 1.0f, 0.0f));
}

Matrix4f CalculateLightSpaceMatrix(Vector3f CameraPos, Vector3f CameraFront, Vector3f LightDirection, float Near, float Far)
{
    std::vector<Vector4f> Corners = GetFustrumCornersWorldSpace(CameraPos, CameraFront, Near, Far);
    Matrix4f LightViewMatrix = CalculateLightViewMatrix(Corners, LightDirection);

    /*
        Transform fustrum corner points in light view space to find maximum and minimum coordinates
        In this viewpoint, fustrum is series of rectangles
    */

    float MinX = std::numeric_limits<float>::max();
    float MaxX = std::numeric_limits<float>::lowest();
    float MinY = std::numeric_limits<float>::max();
    float MaxY = std::numeric_limits<float>::lowest();
    float MinZ = std::numeric_limits<float>::max();
    float MaxZ = std::numeric_limits<float>::lowest();

    for (const auto Vertex : Corners)
    {
        Vector4f TransforedVertex = LightViewMatrix.Multiply(Vertex);
        MinX = std::min(MinX, TransforedVertex.x);
        MaxX = std::max(MaxX, TransforedVertex.x);
        MinY = std::min(MinY, TransforedVertex.y);
        MaxY = std::max(MaxY, TransforedVertex.y);
        MinZ = std::min(MinZ, TransforedVertex.z);
        MaxZ = std::max(MaxZ, TransforedVertex.z);
    }

    // Increase size of space covered by pulling back near plane and pushing far plane
    constexpr float ZMultipier = 1.0f;
    MinZ = MinZ < 0 ? MinZ * ZMultipier : MinZ / ZMultipier;
    MaxZ = MaxZ < 0 ? MaxZ / ZMultipier : MaxZ * ZMultipier;

    

    return LightViewMatrix.Multiply(CreateOrthographicProjectionMatrix(MinY, MaxY, MinX, MaxX, MinZ, MaxZ));


}
