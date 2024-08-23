#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/Player.h"

Player *Player::GetInstance(Vector3f position, Vector3f velocity)
{
    if (Player_ == nullptr)
        Player_ = new Player(position, velocity);
    return Player_;
}

Player *Player::GetInstance()
{
    if (Player_ == nullptr)
    {
        std::cout << "Error: Player singleton is not initialised" << std::endl;
        std::exit(0);
    }

    return Player_;
}

Player::Player(Vector3f position, Vector3f velocity) : Entity(position, velocity)
{
}

void Player::HandleInput(GLFWwindow *Window, float DeltaTime)
{
    Vector3f Direction;

    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
    {
        Direction = camera.CameraFront;
    }
    else if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
    {
        Direction = camera.CameraFront.Multiply(-1.0f);
    }
    else if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
    {
        Direction = camera.CameraFront.CrossProduct(camera.CameraFront, camera.Up).ReturnNormalise().Multiply(-1.0f);
    }
    else if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
    {
        Direction = camera.CameraFront.CrossProduct(camera.CameraFront, camera.Up).ReturnNormalise();
    }
    else if (glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        Direction = camera.Up;
    }
    else if (glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        Direction = camera.Up.Multiply(-1.0f);
    }

    Velocity = Direction.Multiply(DeltaTime * 320.0f);
}

void Player::Update(float DeltaTime)
{
    Position = Position.Add(Velocity.Multiply(DeltaTime));
    camera.SetCameraPos(Position);
}