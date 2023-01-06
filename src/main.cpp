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

void MouseCallback(GLFWwindow *window, double xpos, double ypos);

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float ConvertToRadians(float Degrees)
{
    return Degrees * 3.14159 / 180;
}

static Camera CameraController = Camera(Vector3f(0.0f, 40.0f, -40.0f), Vector3f(0.0f, 0.0f, 0.0f));

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialise GLAD";
        return -1;
    }

    glViewport(0, 0, 800, 600); // Set size of window for rendering

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseCallback);

    glEnable(GL_DEPTH_TEST);

    // Setup shaders
    Shader ChunkShader = Shader();
    ChunkShader.AddShader("shaders/shader.vs", GL_VERTEX_SHADER);
    ChunkShader.AddShader("shaders/shader.fs", GL_FRAGMENT_SHADER);
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

    // Setup depth map
    unsigned int DepthMapFBO;
    glGenFramebuffers(1, &DepthMapFBO);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024; // Resolution

    unsigned int DepthMap; // used as the FB's depth buffer
    glGenTextures(1, &DepthMap);
    glBindTexture(GL_TEXTURE_2D, DepthMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Setup matrices
    Matrix4f ProjectionMatrix = CreatePerspectiveProjectionMatrix(ConvertToRadians(45), 800.0f / 600.0f, 0.1f, 100.0f);
    Matrix4f ModelMatrix = Matrix4f(1);
    Matrix4f SlimViewMatrix = CameraController.RetrieveSlimLookAtMatrix();

    // Setup world
    Skybox skybox = Skybox();

    std::vector<Chunk> ChunkList;
    std::vector<Matrix4f> PositionList;

    const int size = 10 * CHUNK_SIZE;
    int Heightmap[160][160];

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            Matrix4f ModelMatrix = Matrix4f(1);
            ModelMatrix.Translate(Vector3f(i * CHUNK_SIZE, 0, j * CHUNK_SIZE));

            Chunk NewChunk = Chunk(Vector3f(i, 0, j), Heightmap);
            NewChunk.CreateMesh();
            ChunkList.push_back(NewChunk);

            PositionList.push_back(ModelMatrix);
        }
    }

    std::vector<Tree> TreeList;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> WorldRange(0, 160);

    for (int i = 0; i < 25; i++)
    {
        auto PosX = WorldRange(gen);
        auto PosZ = WorldRange(gen);
        auto Height = Heightmap[PosZ][PosX];

        Tree tree = Tree(Vector3f(PosX, Height, PosZ));
        tree.CreateMesh();

        TreeList.push_back(tree);
    }

    Vector3f DLightDirection = Vector3f(0.0f, -1.0f, 0.0f);
    Vector3f DLightingAmbient = Vector3f(0.2f, 0.2f, 0.2f);
    Vector3f DLightingDiffuse = Vector3f(0.7f, 0.7f, 0.7f);
    Vector3f DLightingSpecular = Vector3f(1.0f, 1.0f, 1.0f);

    Vector3f CameraViewPosition = CameraController.GetCameraPos();
    Vector3f CameraDirection = CameraController.CameraFront;

    unsigned int counter = 0;

    float NearPlane = 1.0f, FarPlane = 7.5f;
    glm::mat4 LightProjectionMatrix = glm::ortho(-160.0f, 160.0f, -160.0f, 160.0f, NearPlane, FarPlane);
    glm::mat4 LightViewMatrix = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 LightSpaceMatrix = LightProjectionMatrix * LightViewMatrix;

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Handle timing and FPS
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

        // Input
        CameraController.Move(window, deltaTime, Heightmap);

        // Setup Matrices
        glm::mat4 ViewTestMatrix = CameraController.TestLookAt();
        SlimViewMatrix = CameraController.RetrieveSlimLookAtMatrix();
        CameraViewPosition = CameraController.GetCameraPos();
        CameraDirection = CameraController.CameraFront;

        // Begin rendering

        // Render to depth map
        // glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        // glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
        // glClear(GL_DEPTH_BUFFER_BIT);

        // for (unsigned i = 0; i < ChunkList.size(); i++)
        // {
        //     auto TestChunk = ChunkList[i];
        //     auto PositionThing = PositionList[i];

        //     DepthShader.Use();
        //     DepthShader.SetMatrix4f("model", (const float *)(&PositionThing));
        //     DepthShader.SetMatrix4f("lightSpaceMatrix", (const float *)(&LightSpaceMatrix));

        //     TestChunk.Draw(&DepthShader, true);
        // }

        // for (unsigned i = 0; i < TreeList.size(); i++)
        // {
        //     DepthShader.Use();
        //     DepthShader.SetMatrix4f("lightSpaceMatrix", (const float *)(&LightSpaceMatrix));
        //     TreeList[i].Draw(&DepthShader, true);
        // }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render scene with shadow mapping
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // Setup textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureAtlas::GetInstance()->GetTextureAtlasId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DepthMap);

        for (unsigned i = 0; i < ChunkList.size(); i++)
        {
            auto TestChunk = ChunkList[i];
            auto PositionThing = PositionList[i];

            ChunkShader.Use();

            ChunkShader.SetInt("DiffuseTexture0", 0);
            ChunkShader.SetInt("ShadowMap", 1);
            ChunkShader.SetFloat("NumberOfRows", 16.0f);

            ChunkShader.SetVector3f("ViewPosition", &CameraViewPosition);
            ChunkShader.SetVector3f("DirectionalLight.direction", &DLightDirection);
            ChunkShader.SetVector3f("DirectionalLight.ambient", &DLightingAmbient);
            ChunkShader.SetVector3f("DirectionalLight.diffuse", &DLightingDiffuse);
            ChunkShader.SetVector3f("DirectionalLight.specular", &DLightingSpecular);

            ChunkShader.SetMatrix4f("lightSpaceMatrix", (const float *)(&LightSpaceMatrix));

            ChunkShader.SetMatrix4f("model", (const float *)(&PositionThing));
            ChunkShader.SetMatrix4f("view", (const float *)(&ViewTestMatrix));
            ChunkShader.SetMatrix4f("projection", (const float *)(&ProjectionMatrix));

            TestChunk.Draw(&ChunkShader, false);
        }

        for (unsigned i = 0; i < TreeList.size(); i++)
        {
            ChunkShader.Use();

            ChunkShader.SetInt("DiffuseTexture0", 0);
            ChunkShader.SetFloat("NumberOfRows", 16.0f);

            ChunkShader.SetVector3f("ViewPosition", &CameraViewPosition);
            ChunkShader.SetVector3f("DirectionalLight.direction", &DLightDirection);
            ChunkShader.SetVector3f("DirectionalLight.ambient", &DLightingAmbient);
            ChunkShader.SetVector3f("DirectionalLight.diffuse", &DLightingDiffuse);
            ChunkShader.SetVector3f("DirectionalLight.specular", &DLightingSpecular);

            ChunkShader.SetMatrix4f("lightSpaceMatrix", (const float *)(&LightSpaceMatrix));

            ChunkShader.SetMatrix4f("view", (const float *)(&ViewTestMatrix));
            ChunkShader.SetMatrix4f("projection", (const float *)(&ProjectionMatrix));

            TreeList[i].Draw(&ChunkShader, false);
        }

        SkyboxShader.Use();
        SkyboxShader.SetMatrix4f("view", (const float *)(&SlimViewMatrix));
        SkyboxShader.SetMatrix4f("projection", (const float *)(&ProjectionMatrix));
        skybox.Draw(&SkyboxShader);

        glfwSwapBuffers(window); // Presemably uses double buffering thus swaps front and back buffers
        glfwPollEvents();        // Checks for events (mouse, keyboard) and updates state and
    }

    glfwTerminate();
    return 0;
}

float LastX = 400;
float LastY = 300;
float Yaw = -90.0f;
float Pitch = 0;

bool firstMouse = true;

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

    Vector3f Direction = Vector3f(x, 0, z);
    Direction.Normalise();

    CameraController.CameraFront = Direction;
}
