#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <iostream>

#include "world.h" // Includes Mesh, Shader, Matrix, Chunk
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

static Camera CameraController = Camera(Vector3f(-16.0f, 16.0f, -16.0f), Vector3f(0.0f, 0.0f, 0.0f));

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
    Shader ChunkShader = Shader();
    ChunkShader.AddShader("shader.vs", GL_VERTEX_SHADER);
    ChunkShader.AddShader("shader.fs", GL_FRAGMENT_SHADER);
    ChunkShader.LinkShader();

    // Setup matrices
    Matrix4f ProjectionMatrix = CreatePerspectiveProjectionMatrix(ConvertToRadians(45), 800.0f / 600.0f, 0.1f, 100.0f);
    Matrix4f ViewMatrix = Matrix4f(1);

    std::vector<Chunk> ChunkList;
    std::vector<Matrix4f> PositionList;

    for (int i = -5; i < 5; i++)
    {
        for (int j = -5; j < 5; j++)
        {
            Chunk NewChunk = Chunk();
            ChunkList.emplace_back(NewChunk);

            Matrix4f ModelMatrix = Matrix4f(1);
            ModelMatrix.Translate(Vector3f(i * CHUNK_SIZE, 0, j * CHUNK_SIZE));

            PositionList.emplace_back(ModelMatrix);
        }
    }

    Skybox::Create();

    Matrix4f ModelMatrix = Matrix4f(1);

    ViewMatrix = CameraController.RetrieveLookAt();
    Matrix4f SlimViewMatrix = CameraController.RetrieveSlimLookAtMatrix();

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

        ViewMatrix = CameraController.RetrieveLookAt();
        SlimViewMatrix = CameraController.RetrieveSlimLookAtMatrix();

        for (unsigned i = 0; i < ChunkList.size(); i++)
        {
            auto TestChunk = ChunkList[i];
            auto PositionThing = PositionList[i];

            ChunkShader.Use();
            ChunkShader.SetMatrix4f("model", (const float *)(&PositionThing));
            ChunkShader.SetMatrix4f("view", (const float *)(&ViewMatrix));
            ChunkShader.SetMatrix4f("projection", (const float *)(&ProjectionMatrix));
            TestChunk.Draw(&ChunkShader);
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

float HorizontalAngle = 3.14f;
float VerticalAngle = 0.0f;

double xpos_old, ypos_old;

void MouseCallback(GLFWwindow *window, double XPos, double YPos)
{
    // glfwSetCursorPos(800 / 2, 600 / 2);

    // HorizontalAngle += 0.005f * deltaTime * float(800 / 2 - XPos);
    // VerticalAngle += 0.005f * deltaTime * float(600 / 2 - YPos);

    HorizontalAngle += 0.005f * float(xpos_old - XPos);
    VerticalAngle += 0.005f * float(ypos_old - YPos);
    xpos_old = XPos;
    ypos_old = YPos;

    Vector3f NewDirection = Vector3f(cos(VerticalAngle) * sin(HorizontalAngle), sin(VerticalAngle), cos(VerticalAngle) * cos(HorizontalAngle));
    NewDirection.Normalise();

    CameraController.CameraFront = NewDirection;
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
