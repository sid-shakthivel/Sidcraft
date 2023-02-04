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
#include "../include/Quad.h"
#include "../include/World.h"

void MouseCallback(GLFWwindow *window, double xpos, double ypos);
float ConvertToRadians(float Degrees);
void HandleFPS(GLFWwindow *window);

const unsigned int SCREEN_WIDTH = 800 * 4;
const unsigned int SCREEN_HEIGHT = 600 * 4;

float deltaTime = 0.0f;   // Time between current frame and last frame
float lastFrame = 0.0f;   // Time of last frame
unsigned int counter = 0; //

static Camera CameraController = Camera(Vector3f(0.0f, 15.0f, -40.0f), Vector3f(0.0f, 0.0f, 0.0f));

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
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Setup shaders
    std::cout << "LOADING SHADOW Shader" << std::endl;

    Shader GeneralShader = Shader();
    GeneralShader.AddShader("shaders/shadow.vs", GL_VERTEX_SHADER);
    GeneralShader.AddShader("shaders/shadow.fs", GL_FRAGMENT_SHADER);
    GeneralShader.LinkShader();

    std::cout << "LOADING SKYBOX Shader" << std::endl;

    Shader SkyboxShader = Shader();
    SkyboxShader.AddShader("shaders/SkyboxShader.vs", GL_VERTEX_SHADER);
    SkyboxShader.AddShader("shaders/SkyboxShader.fs", GL_FRAGMENT_SHADER);
    SkyboxShader.LinkShader();

    std::cout << "LOADING DEPTH Shader" << std::endl;

    Shader DepthShader = Shader();
    DepthShader.AddShader("shaders/DepthShader.vs", GL_VERTEX_SHADER);
    DepthShader.AddShader("shaders/DepthShader.fs", GL_FRAGMENT_SHADER);
    DepthShader.LinkShader();

    std::cout << "LOADING HDR " << std::endl;

    Shader HDRShader = Shader();
    HDRShader.AddShader("shaders/HDRShader.vs", GL_VERTEX_SHADER);
    HDRShader.AddShader("shaders/HDRShader.fs", GL_FRAGMENT_SHADER);
    HDRShader.LinkShader();

    std::cout << "LOADING BLUR Shader" << std::endl;

    Shader BlurShader = Shader();
    BlurShader.AddShader("shaders/BlurShader.vs", GL_VERTEX_SHADER);
    BlurShader.AddShader("shaders/BlurShader.fs", GL_FRAGMENT_SHADER);
    BlurShader.LinkShader();

    std::cout << "LOADING BLEND Shader" << std::endl;

    Shader BlendShader = Shader();
    BlendShader.AddShader("shaders/BlendShader.vs", GL_VERTEX_SHADER);
    BlendShader.AddShader("shaders/BlendShader.fs", GL_FRAGMENT_SHADER);
    BlendShader.LinkShader();

    std::cout << "LOADING QUAD Shader" << std::endl;

    Shader QuadShader = Shader();
    QuadShader.AddShader("shaders/QuadShader.vs", GL_VERTEX_SHADER);
    QuadShader.AddShader("shaders/QuadShader.fs", GL_FRAGMENT_SHADER);
    QuadShader.LinkShader();

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

    // Setup world
    World NewWorld = World();

    // Setup required matrices and vectors
    glm::mat4 lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, 1.0f, 7.5f);
    // glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 9.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));

    glm::vec3 GLMlightDir = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::vec3 position = glm::vec3(2.0f, 3.0f, -4.0f);
    glm::mat4 lightView = glm::lookAt(position, position + GLMlightDir, glm::vec3(0.0, 1.0, 0.0));

    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.01f, 1000.0f);
    glm::mat4 view = CameraController.TestLookAt();
    Vector3f CameraViewPosition = CameraController.GetCameraPos();
    Matrix4f SlimViewMatrix = CameraController.RetrieveSlimLookAtMatrix();

    GeneralShader.Use();
    GeneralShader.SetInt("diffuseTexture", 0);
    GeneralShader.SetInt("shadowMap", 1);

    Vector3f LightDir = Vector3f(-0.2f, -1.0f, -0.3f);

    Quad ViewQuad = Quad();

    Vector3f SkyColour = Vector3f(0.5f, 0.5f, 0.5f);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        HandleFPS(window);

        CameraController.Move(window, deltaTime, Heightmap);

        // Update key variables
        CameraViewPosition = CameraController.GetCameraPos();
        view = CameraController.TestLookAt();
        SlimViewMatrix = CameraController.RetrieveSlimLookAtMatrix();

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GeneralShader.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureAtlas::GetInstance()->GetTextureAtlasId());

        GeneralShader.SetInt("diffuseTexture", 0);

        GeneralShader.SetVector3f("viewPos", &CameraViewPosition);
        GeneralShader.SetVector3f("lightPos", &LightDir);

        GeneralShader.SetVector3f("SkyColour", &SkyColour);

        GeneralShader.SetMatrix4f("lightSpaceMatrix", (const float *)(&lightSpaceMatrix));
        GeneralShader.SetMatrix4f("view", (const float *)(&view));
        GeneralShader.SetMatrix4f("projection", (const float *)(&projection));

        for (auto const &[Offset, Chunk] : NewWorld.TerrainData)
            Chunk.Draw(&GeneralShader, false, Offset);

        for (unsigned i = 0; i < NewWorld.TreeList.size(); i++)
            NewWorld.TreeList[i].Draw(&GeneralShader, false);

        SkyboxShader.Use();
        SkyboxShader.SetMatrix4f("view", (const float *)(&SlimViewMatrix));
        SkyboxShader.SetMatrix4f("projection", (const float *)(&projection));
        SkyboxShader.SetVector3f("FogColour", &SkyColour);
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