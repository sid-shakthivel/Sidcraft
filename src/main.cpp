#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <iostream>
#include <random>
#include <string>
#include <tuple>
#include <mach/mach.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../include/Matrix.h"
#include "../include/Camera.h"
#include "../include/Shader.h"
#include "../include/TextureAtlas.h"
#include "../include/Chunk.h"
#include "../include/Skybox.h"
#include "../include/Quad.h"
#include "../include/World.h"
#include "../include/MouseHandler.h"
#include "../include/Renderer.h"

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

float DeltaTime = 0.0f; // Time between current frame and last frame
float LastFrame = 0.0f; // Time of last frame
float LastFPSTime = 0.0f;
float FPS = 0.0f;

// unsigned int WINDOW_WIDTH = 1440;
// unsigned int WINDOW_HEIGHT = 847;

unsigned int SCREEN_WIDTH;
unsigned int SCREEN_HEIGHT;

unsigned int WINDOW_WIDTH;
unsigned int WINDOW_HEIGHT;

int GetMemoryUsage()
{
    mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) != KERN_SUCCESS)
    {
        std::cerr << "Failed to get task info" << std::endl;
        return 0.0f;
    }

    return (int)(info.resident_size / (1024 * 1024));
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Provides ability to set hints for createNewWindow
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core gives access to smaller subset of features
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Just for MacOS to work

    GLFWwindow *window = glfwCreateWindow(800, 600, "Sidcraft", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Error: Window initialisation failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    int FramebufferHeight, FramebufferWidth;
    glfwGetFramebufferSize(window, &FramebufferWidth, &FramebufferHeight);

    int WindowHeight, WindowWidth;
    glfwGetWindowSize(window, &WindowWidth, &WindowHeight);

    SCREEN_WIDTH = FramebufferWidth;
    SCREEN_HEIGHT = FramebufferHeight;
    WINDOW_WIDTH = WindowWidth;
    WINDOW_HEIGHT = WindowHeight;

    // Fix sizes for MacOS retina displays
    // if (FramebufferWidth > 800 && FramebufferHeight > 600)
    // {
    //     SCREEN_WIDTH = 2880;
    //     SCREEN_HEIGHT = 1694;
    //     WINDOW_WIDTH = 1440;
    //     WINDOW_HEIGHT = 847;
    // }

    glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);

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
    Shader DepthShader = Shader(std::string("DepthShader"), true);
    Shader HDRShader = Shader(std::string("HDRShader"));
    Shader BlurShader = Shader(std::string("BlurShader"));
    Shader BlendShader = Shader(std::string("BlendShader"));
    Shader QuadShader = Shader(std::string("QuadShader"));
    Shader SkyboxShader = Shader(std::string("SkyShader"));
    Shader WaterShader = Shader(std::string("WaterShader"));

    // Setup textures
    TextureAtlas::GetInstance();

    // Setup other
    MainMouseHandler.Initialise();
    Camera::GetInstance(Vector3f(120.0f, 45.0f, 120.0f), Vector3f(0.0f, 0.0f, -1.0f));
    Renderer MasterRenderer = Renderer();
    World::GetInstance();

    Quad StartQuad = Quad(Vector3f(1.0f, 0.5f, 0.5f));
    Quad FinalQuad = Quad();
    Quad DebugQuad = Quad();

    bool HasStartedGame = true;

    // Setup rendering process
    MasterRenderer.SetupDepth();
    MasterRenderer.SetupMatrixUBO();

    MasterRenderer.SetupHDR();
    MasterRenderer.SetupBloom();

    MasterRenderer.SetupReflection();
    MasterRenderer.SetupRefraction();

    MasterRenderer.SetupTextures();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    // Render loop
    while (!glfwWindowShouldClose(window))
    {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Sidcraft");
        ImGui::Text("%s", ("FPS: " + std::to_string((int)FPS) + "MS").c_str());
        ImGui::Text("%s", ("Memory Usage: " + std::to_string(GetMemoryUsage()) + " MB").c_str());
        ImGui::End();

        if (!HasStartedGame)
        {
            glClearColor(0.22f, 0.451f, 0.282f, 1.0f);

            MasterRenderer.DrawTempQuad(&QuadShader, &StartQuad);

            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
                HasStartedGame = true;
        }
        else
        {
            float StartTime = glfwGetTime();

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

            MasterRenderer.Update();
            Camera::GetInstance()->Move(window, DeltaTime, World::GetInstance()->Heightmap);

            // Render depth
            MasterRenderer.UBOPass();
            MasterRenderer.RenderDepth(&DepthShader);

            glDisable(GL_CULL_FACE);

            // Setup reflection and refraction planes
            glEnable(GL_CLIP_DISTANCE0);

            float Distance = 2 * (Camera::GetInstance()->GetCameraPos().y - WATER_LEVEL);

            Camera::GetInstance()->CameraPos.y -= Distance;
            Camera::GetInstance()->CameraPos.z -= Distance;
            Camera::GetInstance()->InvertPitch();
            MasterRenderer.RenderReflection(&MainShader);

            Camera::GetInstance()->CameraPos.y += Distance;
            Camera::GetInstance()->CameraPos.z += Distance;
            Camera::GetInstance()->InvertPitch();
            MasterRenderer.RenderRefraction(&MainShader);

            glDisable(GL_CLIP_DISTANCE0);

            // Render everything
            MasterRenderer.RenderNormal(&MainShader, LastFPSTime);
            MasterRenderer.RenderHDR(&MainShader, LastFPSTime);
            MasterRenderer.DrawSkybox(&SkyboxShader, DeltaTime);
            MasterRenderer.RenderWater(&WaterShader, DeltaTime);
            MasterRenderer.RenderBlur(&BlurShader, &FinalQuad);
            MasterRenderer.RenderBloom(&BlendShader, &FinalQuad);

            // Handle FPS

            float EndTime = glfwGetTime();
            DeltaTime = EndTime - StartTime;

            if (glfwGetTime() - LastFPSTime >= 1.0)
            {
                FPS = int(1.0 / DeltaTime);
                LastFPSTime = glfwGetTime();
                // std::cout << "FPS: " << Time << "ms" << std::endl;
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window); // Uses double buffering thus swaps front and back buffers
        glfwPollEvents();        // Checks for events (mouse, keyboard) and updates state and
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
