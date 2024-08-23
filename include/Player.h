#pragma once

#include "Entity.h"
#include "Camera.h"

static const Vector3f DEFAULT_CAMERA_POS = Vector3f(120.0f, 45.0f, 120.0f);
static const Vector3f DEFAULT_CAMERA_TARGET = Vector3f(0.0f, 0.0f, -1.0f);

class Player : public Entity
{
protected:
    Player(Vector3f position, Vector3f velocity);
    static Player *Player_;

public:
    Camera camera = Camera(DEFAULT_CAMERA_POS, DEFAULT_CAMERA_TARGET);

    Player(Player &other) = delete;
    void operator=(const Player &) = delete;

    static Player *GetInstance(Vector3f position, Vector3f velocity);
    static Player *GetInstance();

    void HandleInput(GLFWwindow *Window, float DeltaTime);
    void Update(float DeltaTime);
};

inline Player *Player::Player_ = nullptr;