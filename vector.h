#pragma once

#include <math.h>
#include "matrix.h"

template <class T, unsigned int N>
class Vector
{
private:
    static constexpr auto Size = N;

public:
    T x;
    T y;
    T z;

    Vector(T _x, T _y)
    {
        x = _x;
        y = _y;
    }

    Vector(T _x, T _y, T _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    T GetMagnitude()
    {
        sqrt(pow(x, 2) + pow(y, 2) + pow(z, 0));
    }

    void Add(Vector VecB)
    {
        x += VecB.x;
        y += VecB.y;
        z += VecB.z;
    }

    void Add(T Scalar)
    {
        x, y, z += Scalar;
    }

    void Sub(Vector VecB)
    {
        if (VecB.Size == Size)
        {
            x -= VecB.x;
            y -= VecB.y;
            z -= VecB.z;
        }
    }

    void Sub(T Scalar)
    {
        x, y, z -= Scalar;
    }

    T DotProduct()
    {
    }

    T CrossProduct()
    {
    }

    void Normalise()
    {
        T magnitude = GetMagnitude();
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
    }

    void Multiply(SquareMatrix Matrix)
    {
        if (Matrix.Size != Size)
            return;

        // TODO: Work on this...
        T finalX, finalY, finalZ = 0;

        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
                finalX += x * Matrix.elements[i][k];
    }
};

typedef Vector<float, 3> Vector3f;
typedef Vector<float, 2> Vector2f;