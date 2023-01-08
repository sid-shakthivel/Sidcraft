#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <iostream>
#include <random>

#include "../include/Matrix.h"
#include "../include/Camera.h"
#include "../include/Shader.h"
#include "../include/TextureAtlas.h"
#include "../include/Tree.h"
#include "../include/Chunk.h"
#include "../include/Skybox.h"
#include "../include/World.h"

void MouseCallback(GLFWwindow *window, double xpos, double ypos);
float ConvertToRadians(float Degrees);
void HandleFPS(GLFWwindow *window);

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

float deltaTime = 0.0f;   // Time between current frame and last frame
float lastFrame = 0.0f;   // Time of last frame
unsigned int counter = 0; //

static Camera CameraController = Camera(Vector3f(0.0f, 30.0f, -20.0f), Vector3f(0.0f, 0.0f, 0.0f));

int main()
{
    int Heightmap[160][160];

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialise GLAD";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Setup shaders
    Shader ChunkShader = Shader();
    ChunkShader.AddShader("shaders/shadow.vs", GL_VERTEX_SHADER);
    ChunkShader.AddShader("shaders/shadow.fs", GL_FRAGMENT_SHADER);
    ChunkShader.LinkShader();

    Shader SkyboxShader = Shader();
    SkyboxShader.AddShader("shaders/SkyboxShader.vs", GL_VERTEX_SHADER);
    SkyboxShader.AddShader("shaders/SkyboxShader.fs", GL_FRAGMENT_SHADER);
    SkyboxShader.LinkShader();

    Shader DepthShader = Shader();
    DepthShader.AddShader("shaders/DepthShader.vs", GL_VERTEX_SHADER);
    DepthShader.AddShader("shaders/DepthShader.fs", GL_FRAGMENT_SHADER);
    DepthShader.LinkShader();

    // Setup textures
    TextureAtlas::GetInstance();

    // Setup Depth Map
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int DepthMapFBO;
    glGenFramebuffers(1, &DepthMapFBO);

    unsigned int DepthMap;
    glGenTextures(1, &DepthMap);
    glBindTexture(GL_TEXTURE_2D, DepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Setup world
    World NewWorld = World();

    // Setup required matrices and vectors
    glm::mat4 lightProjection = glm::ortho(-800.0f, 800.0f, -800.0f, 800.0f, 1.0f, 7.0f);
    glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f, 30.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.01f, 1000.0f);
    glm::mat4 view = CameraController.TestLookAt();
    Vector3f CameraViewPosition = CameraController.GetCameraPos();
    Matrix4f SlimViewMatrix = CameraController.RetrieveSlimLookAtMatrix();

    ChunkShader.Use();
    ChunkShader.SetInt("diffuseTexture", 0);
    ChunkShader.SetInt("shadowMap", 1);

    Vector3f LightDir = Vector3f(-0.2f, -1.0f, -0.3f);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        HandleFPS(window);

        CameraController.Move(window, deltaTime, Heightmap);

        // Update key variables
        CameraViewPosition = CameraController.GetCameraPos();
        view = CameraController.TestLookAt();
        SlimViewMatrix = CameraController.RetrieveSlimLookAtMatrix();

        // Begin rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render to depth map
        DepthShader.Use();
        DepthShader.SetMatrix4f("lightSpaceMatrix", (const float *)(&lightSpaceMatrix));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureAtlas::GetInstance()->GetTextureAtlasId());

        for (auto const &[Offset, Chunk] : NewWorld.TerrainData)
            Chunk.Draw(&DepthShader, true, Offset);

        for (unsigned i = 0; i < NewWorld.TreeList.size(); i++)
            NewWorld.TreeList[i].Draw(&DepthShader, true);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render scene with shadow mapping
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ChunkShader.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureAtlas::GetInstance()->GetTextureAtlasId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DepthMap);

        ChunkShader.SetInt("diffuseTexture", 0);
        ChunkShader.SetInt("shadowMap", 1);

        ChunkShader.SetVector3f("viewPos", &CameraViewPosition);
        ChunkShader.SetVector3f("lightPos", &LightDir);

        ChunkShader.SetMatrix4f("lightSpaceMatrix", (const float *)(&lightSpaceMatrix));
        ChunkShader.SetMatrix4f("view", (const float *)(&view));
        ChunkShader.SetMatrix4f("projection", (const float *)(&projection));

        for (auto const &[Offset, Chunk] : NewWorld.TerrainData)
            Chunk.Draw(&ChunkShader, false, Offset);

        for (unsigned i = 0; i < NewWorld.TreeList.size(); i++)
            NewWorld.TreeList[i].Draw(&ChunkShader, false);

        SkyboxShader.Use();
        SkyboxShader.SetMatrix4f("view", (const float *)(&SlimViewMatrix));
        SkyboxShader.SetMatrix4f("projection", (const float *)(&projection));
        NewWorld.skybox.Draw(&SkyboxShader, deltaTime);

        glfwSwapBuffers(window); // Uses double buffering thus swaps front and back buffers
        glfwPollEvents();        // Checks for events (mouse, keyboard) and updates state and
    }

    glfwTerminate();
    return 0;
}

float LastX = 400;
float LastY = 300;
float Yaw = 90.0f;
float Pitch = 0;

bool firstMouse = true;

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

void MouseCallback(GLFWwindow *window, double XPos, double YPos)
{
    if (firstMouse)
    {
        LastX = XPos;
        LastY = YPos;
        firstMouse = false;
    }

    // Calculate offset
    float XOffset = XPos - LastX;
    float YOffset = LastY - YPos;
    LastX = XPos;
    LastY = YPos;

    float sensitivity = 0.1f;
    XOffset *= sensitivity;
    YOffset *= sensitivity;

    Yaw += XOffset;
    Pitch += YOffset;

    // Constrain the value
    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    float x = cos(ConvertToRadians(Yaw)) * cos(ConvertToRadians(Pitch));
    float y = sin(ConvertToRadians(Pitch));
    float z = sin(ConvertToRadians(Yaw)) * cos(ConvertToRadians(Pitch));

    CameraController.CameraFront = Vector3f(x, 0, z).ReturnNormalise();
}

float ConvertToRadians(float Degrees)
{
    return Degrees * 3.14159 / 180;
}