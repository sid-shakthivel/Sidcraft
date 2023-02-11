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

void MouseCallback(GLFWwindow *window, double xpos, double ypos);
void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void HandleFPS(GLFWwindow *window);

static MouseHandler MainMouseHandler = MouseHandler();
static glm::mat4 TestProjection;
static glm::mat4 TestView;

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
    Shader GeneralShader = Shader(std::string("shadow"));
    Shader DepthShader = Shader(std::string("DepthShader"));
    Shader HDRShader = Shader(std::string("HDRShader"));
    Shader BlurShader = Shader(std::string("BlurShader"));
    Shader BlendShader = Shader(std::string("BlendShader"));
    Shader QuadShader = Shader(std::string("QuadShader"));
    Shader SkyboxShader = Shader(std::string("SkyShader"));

    // Setup textures
    TextureAtlas::GetInstance();

    // Setup framebuffer for HDR
    unsigned int HdrFBO;
    glGenFramebuffers(1, &HdrFBO);

    unsigned int ColourBuffers[2];
    glGenTextures(2, &ColourBuffers[0]);

    glBindFramebuffer(GL_FRAMEBUFFER, HdrFBO);

    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, ColourBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, ColourBuffers[i], 0);
    }

    unsigned int RboDepth;
    glGenRenderbuffers(1, &RboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, RboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, HdrFBO);
    unsigned int Attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, Attachments);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Setup pingpong buffers for blur rendering
    unsigned int PingPongFBO[2];
    unsigned int PingPongBuffers[2];
    glGenFramebuffers(2, &PingPongFBO[0]);
    glGenTextures(2, &PingPongBuffers[0]);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, PingPongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, PingPongBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, PingPongBuffers[i], 0);
    }

    // Setup shadow specific buffers
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    unsigned int DepthMapFBO;
    glGenFramebuffers(1, &DepthMapFBO);

    unsigned int DepthMap;
    glGenTextures(1, &DepthMap);
    glBindTexture(GL_TEXTURE_2D, DepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Camera::GetInstance(Vector3f(0.0f, 25.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f));
    Renderer MasterRenderer = Renderer();
    World::GetInstance();

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        HandleFPS(window);
        MasterRenderer.Update();
        Camera::GetInstance()->Move(window, deltaTime, World::GetInstance()->Heightmap);

        std::tuple<glm::mat4, glm::mat4> Matrices = MasterRenderer.GetMatrices();
        TestProjection = get<0>(Matrices);
        TestView = get<1>(Matrices);

        MasterRenderer.RenderScene(&GeneralShader);
        MasterRenderer.RenderSkybox(&SkyboxShader, deltaTime);

        glfwSwapBuffers(window); // Uses double buffering thus swaps front and back buffers
        glfwPollEvents();        // Checks for events (mouse, keyboard) and updates state and
    }

    glfwTerminate();
    return 0;
}

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

void MouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    // Camera::GetInstance()->Rotate(xpos, ypos);   
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    double XPos, YPos;
    glfwGetCursorPos(window, &XPos, &YPos);

    Vector3f Ray = MainMouseHandler.GetRay(XPos, YPos, TestProjection, TestView);

    Vector3f PositionToTest;

    for (int i = 1; i <= 16; i++)
    {
        PositionToTest = Ray.Multiply(i).Add(Camera::GetInstance()->GetCameraPos().Add(Camera::GetInstance()->CameraFront));

        PositionToTest.x = std::min<float>(PositionToTest.x, 159);
        PositionToTest.z = std::min<float>(PositionToTest.z, 159);

        PositionToTest.x = std::max<float>(PositionToTest.x, 0);
        PositionToTest.z = std::max<float>(PositionToTest.z, 0);

        int X = floor(PositionToTest.x / 16);
        int Z = floor(PositionToTest.z / 16);

        Vector3f TargetOffset = Vector3f(X * 16, 0, Z * 16);

        for (int Index = 0; Index < World::GetInstance()->ChunkData.size(); Index++)
        {
            auto Offset = World::GetInstance()->ChunkPositions.at(Index);
            auto TempChunk = &World::GetInstance()->ChunkData.at(Index);

            if (TempChunk->IsWithinChunk(PositionToTest, Offset))
            {
                auto NewChunk = Chunk(TempChunk->Blocks);

                if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
                    NewChunk.SetChunk(PositionToTest.Sub(Ray), Offset);
                else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
                    NewChunk.ClearChunk(PositionToTest, Offset);

                NewChunk.CreateMesh();

                World::GetInstance()->ChunkData.erase(World::GetInstance()->ChunkData.begin() + Index);
                World::GetInstance()->ChunkPositions.erase(World::GetInstance()->ChunkPositions.begin() + Index);

                World::GetInstance()->ChunkPositions.push_back(Offset);
                World::GetInstance()->ChunkData.push_back(NewChunk);

                break;
            }
        }
    }
}
