#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <iostream>

#include "Chunk.h" // Includes Mesh (contains shader + matrix)
#include "cubemap.h"
#include "camera.h"
// #include "shader.h"

void ProcessInput(GLFWwindow *window, Camera *CameraController);
void MouseCallback(GLFWwindow *window, double xpos, double ypos);

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float ConvertToRadians(float Degrees)
{
    return Degrees * 3.14159 / 180;
}

static Camera CameraController = Camera(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f));

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
    Shader SkyboxShader = Shader();
    SkyboxShader.AddShader("SkyboxShader.vs", GL_VERTEX_SHADER);
    SkyboxShader.AddShader("SkyboxShader.fs", GL_FRAGMENT_SHADER);
    SkyboxShader.LinkShader();

    // Setup shaders
    // Shader ChunkShader = Shader();
    // ChunkShader.AddShader("shader.vs", GL_VERTEX_SHADER);
    // ChunkShader.AddShader("shader.fs", GL_FRAGMENT_SHADER);
    // ChunkShader.LinkShader();

    // Setup matrices
    Matrix4f ProjectionMatrix = CreatePerspectiveProjectionMatrix(ConvertToRadians(45), 800.0f / 600.0f, 0.1f, 100.0f);
    Matrix4f ViewMatrix = Matrix4f(1);
    Matrix4f ModelMatrix = Matrix4f(1);

    // Generate grid of chunks to render - make this into a sort of class eventually

    // std::vector<Chunk> Chunks;
    // for (unsigned int i = 0; i < 3; i++)
    // {
    //     // for (unsigned int j = 0; j < 2; j++)
    //     // {

    //     // }

    //     Chunk exampleChunk = Chunk();
    //     exampleChunk.CreateMesh();

    //     std::for_each(exampleChunk.VertexData.begin(), exampleChunk.VertexData.end(), [i](ThinVertex &TV)
    //                   { TV.Position = TV.Position.Add(Vector3f(i * 12, 0, 0)); });

    //     Chunks.push_back(exampleChunk);
    // }

    // for (unsigned int i = 1; i < 2; i++)
    // {
    //     for (auto v : Chunks[i].VertexData)
    //     {
    //         v.Position.Print();
    //     }
    // }

    Cubemap::Create();

    // Chunk exampleChunk = Chunk();
    // exampleChunk.CreateMesh();

    // Chunk testChunk = Chunk();
    // testChunk.CreateMesh();

    // std::for_each(testChunk.VertexData.begin(), testChunk.VertexData.end(), [](ThinVertex &TV)
    //               { TV.Position = TV.Position.Add(Vector3f(1 * 17, 0, 0)); });

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        ProcessInput(window, &CameraController);

        // Render
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        // glClearColor(0.2f, 0.3f, 0.8f, 1.0f);

        ViewMatrix = CameraController.RetrieveLookAt();

        SkyboxShader.Use();
        SkyboxShader.SetMatrix4f("view", (const float *)(&ViewMatrix));
        SkyboxShader.SetMatrix4f("projection", (const float *)(&ProjectionMatrix));
        Cubemap::Draw(&SkyboxShader);

        // ChunkShader.Use();
        // ChunkShader.SetMatrix4f("model", (const float *)(&ModelMatrix));
        // ChunkShader.SetMatrix4f("view", (const float *)(&ViewMatrix));
        // ChunkShader.SetMatrix4f("projection", (const float *)(&ProjectionMatrix));

        // exampleChunk.Draw(&ChunkShader);
        // testChunk.Draw(&ChunkShader);

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

void ProcessInput(GLFWwindow *window, Camera *CameraController)
{
    float CameraSpeed = deltaTime * 8.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        CameraController->SetCameraPos(CameraController->GetCameraPos().Add(CameraController->CameraFront.Multiply(CameraSpeed)));
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        CameraController->SetCameraPos(CameraController->GetCameraPos().Sub(CameraController->CameraFront.Multiply(CameraSpeed)));
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        Vector3f Direction = CameraController->CameraFront.CrossProduct(CameraController->CameraFront, CameraController->Up);
        Direction.Normalise();
        CameraController->SetCameraPos(CameraController->GetCameraPos().Sub(Direction.Multiply(CameraSpeed)));
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        Vector3f Direction = CameraController->CameraFront.CrossProduct(CameraController->CameraFront, CameraController->Up);
        Direction.Normalise();
        CameraController->SetCameraPos(CameraController->GetCameraPos().Add(Direction.Multiply(CameraSpeed)));
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        Vector3f Direction = CameraController->Up;
        Direction.Normalise();
        CameraController->SetCameraPos(CameraController->GetCameraPos().Add(Direction.Multiply(CameraSpeed)));
    }

    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        // Vector3f Direction = Vector3f(0, -1.0, 0);
        Vector3f Direction = CameraController->Up;
        Direction.Normalise();
        CameraController->SetCameraPos(CameraController->GetCameraPos().Sub(Direction.Multiply(CameraSpeed)));
    }
}
