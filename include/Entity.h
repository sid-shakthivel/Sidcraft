#pragma once

#include "Matrix.h"

class Entity
{
protected:
    Vector3f Position;
    Vector3f Velocity;

    float Width;
    float Height;

public:
    Entity(Vector3f position, Vector3f velocity);
    virtual ~Entity() = default;
};