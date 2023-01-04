#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>
#include <iostream>

#include "Tree.h" // Includes Mesh (contains shader + matrix)
#include "cubemap.h"
#include "camera.h"
#include "TextureAtlas.h"

void MouseCallback(GLFWwindow *window, double xpos, double ypos);

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

    GLFWwindow *window = glfwCreateWindow(800, 600, "GameEngine", NULL, NULL);

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
    ChunkShader.AddShader("shader.vs", GL_VERTEX_SHADER);
    ChunkShader.AddShader("shader.fs", GL_FRAGMENT_SHADER);
    ChunkShader.LinkShader();

    Shader SkyboxShader = Shader();
    SkyboxShader.AddShader("SkyboxShader.vs", GL_VERTEX_SHADER);
    SkyboxShader.AddShader("SkyboxShader.fs", GL_FRAGMENT_SHADER);
    SkyboxShader.LinkShader();

    // Setup textures
    TextureAtlas::GetInstance();

    // Setup matrices
    Matrix4f ProjectionMatrix = CreatePerspectiveProjectionMatrix(ConvertToRadians(45), 800.0f / 600.0f, 0.1f, 100.0f);
    Matrix4f ModelMatrix = Matrix4f(1);
    Matrix4f SlimViewMatrix = CameraController.RetrieveSlimLookAtMatrix();

    // Setup world
    Skybox::Create();

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

    for (int i = 0; i < 30; i++)
    {
        auto PosX = WorldRange(gen);
        auto PosZ = WorldRange(gen);
        auto Height = Heightmap[PosZ][PosX];

        Tree tree = Tree(Vector3f(PosX, Height, PosZ));

        TreeList.push_back(tree);
    }

    Vector3f DLightDirection = Vector3f(0.0f, -1.0f, -1.0f);
    Vector3f DLightingAmbient = Vector3f(0.2f, 0.2f, 0.2f);
    Vector3f DLightingDiffuse = Vector3f(0.7f, 0.7f, 0.7f);
    Vector3f DLightingSpecular = Vector3f(1.0f, 1.0f, 1.0f);

    Vector3f CameraViewPosition = CameraController.GetCameraPos();
    Vector3f CameraDirection = CameraController.CameraFront;

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        CameraController.Move(window, deltaTime, Heightmap);

        // Render
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glm::mat4 ViewTestMatrix = CameraController.TestLookAt();
        SlimViewMatrix = CameraController.RetrieveSlimLookAtMatrix();
        CameraViewPosition = CameraController.GetCameraPos();
        CameraDirection = CameraController.CameraFront;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureAtlas::GetInstance()->GetTextureAtlasId());

        for (unsigned i = 0; i < ChunkList.size(); i++)
        {
            auto TestChunk = ChunkList[i];
            auto PositionThing = PositionList[i];

            ChunkShader.Use();

            ChunkShader.SetInt("DiffuseTexture0", 0);
            ChunkShader.SetFloat("NumberOfRows", 16.0f);

            ChunkShader.SetVector3f("ViewPosition", &CameraViewPosition);
            ChunkShader.SetVector3f("DirectionalLight.direction", &DLightDirection);
            ChunkShader.SetVector3f("DirectionalLight.ambient", &DLightingAmbient);
            ChunkShader.SetVector3f("DirectionalLight.diffuse", &DLightingDiffuse);
            ChunkShader.SetVector3f("DirectionalLight.specular", &DLightingSpecular);

            // ChunkShader.SetVector3f("SpotLight.position", &CameraViewPosition);
            // ChunkShader.SetVector3f("SpotLight.direction", &CameraDirection);
            // ChunkShader.SetVector3f("SpotLight.ambient", &DLightingAmbient);
            // ChunkShader.SetVector3f("SpotLight.diffuse", &DLightingDiffuse);
            // ChunkShader.SetVector3f("SpotLight.specular", &DLightingSpecular);
            // ChunkShader.SetFloat("SpotLight.cutOff", cos(ConvertToRadians(20.5f)));

            ChunkShader.SetMatrix4f("model", (const float *)(&PositionThing));
            ChunkShader.SetMatrix4f("view", (const float *)(&ViewTestMatrix));
            ChunkShader.SetMatrix4f("projection", (const float *)(&ProjectionMatrix));

            TestChunk.Draw(&ChunkShader);
        }

        for (unsigned i = 0; i < TreeList.size(); i++)
        {
            ChunkShader.Use();

            ChunkShader.SetInt("DiffuseTexture0", 0);
            ChunkShader.SetFloat("NumberOfRows", 16.0f);

            ChunkShader.SetVector3f("DirectionalLight.direction", &DLightDirection);
            ChunkShader.SetVector3f("DirectionalLight.ambient", &DLightingAmbient);
            ChunkShader.SetVector3f("DirectionalLight.diffuse", &DLightingDiffuse);
            ChunkShader.SetVector3f("DirectionalLight.specular", &DLightingSpecular);

            ChunkShader.SetMatrix4f("view", (const float *)(&ViewTestMatrix));
            ChunkShader.SetMatrix4f("projection", (const float *)(&ProjectionMatrix));
            TreeList[i].Draw(&ChunkShader);
        }

        SkyboxShader.Use();
        SkyboxShader.SetMatrix4f("view", (const float *)(&SlimViewMatrix));
        SkyboxShader.SetMatrix4f("projection", (const float *)(&ProjectionMatrix));
        Skybox::Draw(&SkyboxShader);

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
