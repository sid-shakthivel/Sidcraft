#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <iostream>
#include <random>
#include <string>
#include <tuple>

#include "../include/Matrix.h"
#include "../include/Camera.h"
#include "../include/Shader.h"
#include "../include/TextureAtlas.h"
#include "../include/Tree.h"
#include "../include/Chunk.h"
#include "../include/Skybox.h"
#include "../include/Quad.h"
#include "../include/World.h"
#include "../include/Renderer.h"
#include "../include/MouseHandler.h"

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void HandleFPS(GLFWwindow *window);

static MouseHandler MainMouseHandler = MouseHandler();
static Matrix4f TestProjection = Matrix4f(1);
static Matrix4f TestView = Matrix4f(1);

float deltaTime = 0.0f;   // Time between current frame and last frame
float lastFrame = 0.0f;   // Time of last frame
unsigned int counter = 0; //

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Provides ability to set hints for createNewWindow
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core gives access to smaller subset of features
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Just for MacOS to work

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GameEngine", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Window initialisation failed";
        glfwTerminate();
        return -1;
    }

    // Update dimensions
    int FramebufferHeight, FramebufferWidth;
    glfwGetFramebufferSize(window, &FramebufferWidth, &FramebufferHeight);

    SCREEN_WIDTH = FramebufferWidth;
    SCREEN_HEIGHT = FramebufferHeight;

    std::cout << SCREEN_WIDTH << " " << SCREEN_HEIGHT << std::endl;

    glfwMakeContextCurrent(window); // Contexts stores all of state assocaiated with this instance of OpenGL
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialise GLAD";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Setup shaders
    Shader MainShader = Shader(std::string("MainShader"));
    Shader DepthShader = Shader(std::string("DepthShader"));
    Shader HDRShader = Shader(std::string("HDRShader"));
    Shader BlurShader = Shader(std::string("BlurShader"));
    Shader BlendShader = Shader(std::string("BlendShader"));
    Shader QuadShader = Shader(std::string("QuadShader"));
    Shader SkyboxShader = Shader(std::string("SkyShader"));
    Shader WaterShader = Shader(std::string("WaterShader"));

    // Setup textures
    TextureAtlas::GetInstance();

    Camera::GetInstance(Vector3f(10.0f, 45.0f, 5.0f), Vector3f(0.0f, 0.0f, -1.0f));
    Renderer MasterRenderer = Renderer();
    World::GetInstance();

    Quad FinalQuad = Quad();

    MasterRenderer.SetupDepth();
    MasterRenderer.SetupReflection();
    MasterRenderer.SetupRefraction();

    bool ShowDepth = false;

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        HandleFPS(window);
        MasterRenderer.Update();
        Camera::GetInstance()->Move(window, deltaTime, World::GetInstance()->Heightmap);

        std::tuple<Matrix4f, Matrix4f> Matrices = MasterRenderer.GetMatrices();
        TestProjection = get<0>(Matrices);
        TestView = get<1>(Matrices);

        glEnable(GL_CLIP_DISTANCE0);

        float Distance = 2 * (Camera::GetInstance()->GetCameraPos().y - WATER_LEVEL);

        Camera::GetInstance()->CameraPos.y -= Distance;
        Camera::GetInstance()->InvertPitch();
        MasterRenderer.RenderReflection(&MainShader);
        MasterRenderer.DrawSkybox(&SkyboxShader, deltaTime);

        Camera::GetInstance()->CameraPos.y += Distance;
        Camera::GetInstance()->InvertPitch();
        MasterRenderer.RenderRefraction(&MainShader);

        glDisable(GL_CLIP_DISTANCE0);

        MasterRenderer.RenderNormal(&MainShader, lastFrame);
        MasterRenderer.RenderWater(&WaterShader, lastFrame);
        MasterRenderer.DrawSkybox(&SkyboxShader, deltaTime);

        // MasterRenderer.DrawLightQuad(&QuadShader, &FinalQuad);

        glfwSwapBuffers(window); // Uses double buffering thus swaps front and back buffers
        glfwPollEvents();        // Checks for events (mouse, keyboard) and updates state and
    }

    glfwTerminate();
    return 0;
}

// if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
//             ShowDepth = false;
//         else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
//             ShowDepth = true;

//         if (!ShowDepth)
//             MasterRenderer.RenderNormal(&MainShader, abs(lastFrame));

//         if (ShowDepth)
//             MasterRenderer.DrawDepthQuad(&QuadShader, &FinalQuad);

void HandleFPS(GLFWwindow *window)
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    counter += 1;

    if (deltaTime >= 1.0 / 30.0)
    {
        std::string FPS = "GameEngine FPS: " + std::to_string((1.0 / deltaTime) * counter) + " MS: " + std::to_string((deltaTime / counter) * 1000);
        glfwSetWindowTitle(window, FPS.c_str());
        counter = 0;
        lastFrame = currentFrame;
    }
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    double XPos, YPos;
    glfwGetCursorPos(window, &XPos, &YPos);

    // Vector3f Ray = MainMouseHandler.GetRay(XPos, YPos, TestProjection, TestView);

    // Vector3f PositionToTest;

    // bool IsFound = false;

    // for (int i = 1; i <= 16; i++)
    // {
    //     PositionToTest = Ray.Multiply(i).Add(Camera::GetInstance()->GetCameraPos().Add(Camera::GetInstance()->CameraFront));

    //     PositionToTest.x = std::min<float>(PositionToTest.x, WORLD_SIZE - 1);
    //     PositionToTest.z = std::min<float>(PositionToTest.z, WORLD_SIZE - 1);

    //     PositionToTest.x = std::max<float>(PositionToTest.x, 0);
    //     PositionToTest.z = std::max<float>(PositionToTest.z, 0);

    //     for (int Index = 0; Index < World::GetInstance()->ChunkData.size(); Index++)
    //     {
    //         auto Offset = World::GetInstance()->ChunkPositions.at(Index);
    //         auto TempChunk = &World::GetInstance()->ChunkData.at(Index);

    //         if (TempChunk->IsWithinChunk(PositionToTest, Offset))
    //         {
    //             auto NewChunk = Chunk(TempChunk->Blocks, TempChunk->LocalHeightmap);

    //             if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    //                 NewChunk.SetChunk(PositionToTest.Sub(Ray), Offset, World::GetInstance()->Heightmap);
    //             else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    //                 NewChunk.ClearChunk(PositionToTest, Offset);

    //             NewChunk.CreateMesh();

    //             World::GetInstance()->ChunkData.erase(World::GetInstance()->ChunkData.begin() + Index);
    //             World::GetInstance()->ChunkPositions.erase(World::GetInstance()->ChunkPositions.begin() + Index);

    //             World::GetInstance()->ChunkPositions.push_back(Offset);
    //             World::GetInstance()->ChunkData.push_back(NewChunk);

    //             IsFound = true;
    //             break;
    //         }
    //     }

    //     if (IsFound)
    //         break;
    // }
}
