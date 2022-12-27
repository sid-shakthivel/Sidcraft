#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <iostream>
#include "shader.h"
#include "stb_image.h"
#include "camera.h" // Includes matrix.h itself

void ProcessInput(GLFWwindow *window, Camera *CameraController);
void MouseCallback(GLFWwindow *window, double xpos, double ypos);

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float ConvertToRadians(float Degrees)
{
    return Degrees * 3.14159 / 180;
}

static Camera CameraController = Camera(Vector3f(0.0f, 0.0f, 3.0f), Vector3f(0.0f, 0.0f, 0.0f));

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

    glEnable(GL_DEPTH_TEST);

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetCursorPosCallback(window, MouseCallback);

    // Load image and create texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);          // (s, t, r) refers to the coordinate axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);          // Mirrors content
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Set filtering method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("chess.jpg", &width, &height, &nrChannels, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    // Setup shaders
    Shader CubeShader = Shader();
    CubeShader.AddShader("shader.vs", GL_VERTEX_SHADER);
    CubeShader.AddShader("shader.fs", GL_FRAGMENT_SHADER);
    CubeShader.LinkShader();

    Shader LightingShader = Shader();
    LightingShader.AddShader("lightingShader.vs", GL_VERTEX_SHADER);
    LightingShader.AddShader("lightingShader.fs", GL_FRAGMENT_SHADER);
    LightingShader.LinkShader();

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f};

    // Create and bind VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO); // First argument specifies how many objects need to be generated
    glBindVertexArray(VAO);     // Binds a VAO which means all subsequent VBO, VertexAttributePointer are stored within the bound VAO

    // Copy vertices array into a buffer for OpenGL
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Setup the vertex attribute pointers which specify how data is read
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // Setup position attribute
    glEnableVertexAttribArray(0);

    // Create and bind VAO
    unsigned int LightVAO;
    glGenVertexArrays(1, &LightVAO); // First argument specifies how many objects need to be generated
    glBindVertexArray(LightVAO);     // Binds a VAO which means all subsequent VBO, VertexAttributePointer are stored within the bound VAO

    // Copy vertices array into a buffer for OpenGL
    unsigned int LightVBO;
    glGenBuffers(1, &LightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, LightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Setup the vertex attribute pointers which specify how data is read
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // Setup position attribute
    glEnableVertexAttribArray(0);

    Vector3f ObjectColour = Vector3f(1.0f, 0.0f, 0.0f);
    Vector3f LightColour = Vector3f(1.0f, 1.0f, 1.0f);

    // Setup matrices
    Matrix4f ModelMatrix = Matrix4f(1);
    Matrix4f ViewMatrix = Matrix4f(1);
    Matrix4f ProjectionMatrix = CreatePerspectiveProjectionMatrix(ConvertToRadians(45), 800.0f / 600.0f, 0.1f, 100.0f);

    ViewMatrix = CameraController.RetrieveLookAt();

    Matrix4f LightModelMatrix = Matrix4f(1);
    Vector3f LightPos = Vector3f(1.2f, 1.0f, 2.0f);
    LightModelMatrix.Translate(LightPos);
    LightModelMatrix.Scale(Vector2f(0.2f, 0.2f));

    // Render loop
    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        // ProcessInput(window, &CameraController);

        // Render
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // Bind textures
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, texture);

        CubeShader.Use();
        // glUniform1i(glGetUniformLocation(CubeShader.programId, "chessTexture"), 0);

        unsigned int transformLoc = glGetUniformLocation(CubeShader.programId, "model");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (const float *)(&ModelMatrix));
        transformLoc = glGetUniformLocation(CubeShader.programId, "view");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (const float *)(&ViewMatrix));
        transformLoc = glGetUniformLocation(CubeShader.programId, "projection");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (const float *)(&ProjectionMatrix));
        CubeShader.SetVector3f("ObjectColour", &ObjectColour);
        CubeShader.SetVector3f("LightColour", &LightColour);

        // Render
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        LightingShader.Use();

        transformLoc = glGetUniformLocation(LightingShader.programId, "model");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (const float *)(&LightModelMatrix));
        transformLoc = glGetUniformLocation(LightingShader.programId, "view");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (const float *)(&ViewMatrix));
        transformLoc = glGetUniformLocation(LightingShader.programId, "projection");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (const float *)(&ProjectionMatrix));

        glBindVertexArray(LightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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

    if (firstMouse) // initially set to true
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

    Vector3f Direction = Vector3f(x, y, z);
    Direction.Normalise();

    CameraController.CameraFront = Direction;
}

void ProcessInput(GLFWwindow *window, Camera *CameraController)
{
    float CameraSpeed = deltaTime * 0.5f;

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
}
