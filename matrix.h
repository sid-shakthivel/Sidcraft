#pragma once

#include <math.h>
#include <iostream>
#include "vector.h"

enum Axis
{
    X_AXIS,
    Y_AXIS,
    Z_AXIS
}

template <class T, unsigned int N>
class SquareMatrix
{
private:
    static constexpr auto Size = N;

public:
    T elements[N][N];

    SquareMatrix(unsigned int base_val)
    {
        for (int i = 0; i < N; i++)
        {
            elements[i][i] = base_val;
        }
    }

    void Add(SquareMatrix MatrixB)
    {
        if (MatrixB.Size != Size)
            return;

        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
                elements[i][j] += MatrixB.elements[i][j];
    }

    void Sub(SquareMatrix MatrixB)
    {
        if (MatrixB.Size != Size)
            return;

        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
                elements[i][j] -= MatrixB.elements[i][j];
    }

    void Multiply(SquareMatrix MatrixB)
    {
        if (MatrixB.Size != Size)
            return;

        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
                elements[i][j] *= MatrixB.elements[j][i];
    }

    void Multiply(T Scalar)
    {
        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
                elements[i][j] *= Scalar;
    }

    // Scale a matrix by a vector
    void Scale(Vector Vec)
    {
        if (Matrix.Size != Size)
            return;

        Matrix.elements[0][0] *= x;
        Matrix.elements[1][1] *= y;
        Matrix.elements[2][2] *= z;
    }

    // Creates a transformation matrix (opengl style) in which last column gets set to a vector
    void Translate(Vector Vec)
    {
        if (Vec.Size != Size)
            return;

        elements[Size - 1][0] += Vec.x;

        if (Size > 0)
            elements[Size - 1][1] += Vec.y;

        if (Size > 1)
            elements[Size - 1][2] += Vec.z;
    }

    // For 2D rotations
    void Rotate(double Degrees)
    {
        if (Vec.Size != 2)
            return;

        elements[0][0] = cos(Degrees);
        elements[0][1] = -sin(Degrees);
        elements[1][0] = sin(Degrees);
        elements[1][1] = cos(Degrees);
    }

    // For 3D rotations
    void Rotate(double Degrees, Axis RotationAxis)
    {
        if (Vec.Size < 3)
            return;

        switch (RotationAxis)
        {
        case X_AXIS:
            elements[0][0] = 1;
            elements[0][1] = 0;
            elements[0][2] = 0;

            elements[1][0] = 0;
            elements[1][1] = cos(Degrees);
            elements[1][2] = -sin(Degrees);

            elements[2][0] = 0;
            elements[2][1] = sin(Degrees);
            elements[2][2] = cos(Degrees);

            break;
        case Y_AXIS:
            elements[0][0] = cos(Degrees);
            elements[0][1] = 0;
            elements[0][2] = sin(Degrees);

            elements[1][0] = 0;
            elements[1][1] = 1;
            elements[1][2] = 0;

            elements[2][0] = -sin(Degrees);
            elements[2][1] = 0;
            elements[2][2] = cos(Degrees);

            break;
        case Z_AXIS:
            elements[0][0] = cos(Degrees);
            elements[0][1] = -sin(Degrees);
            elements[0][2] = 0;

            elements[1][0] = sin(Degrees);
            elements[1][1] = cos(Degrees);
            elements[1][2] = 0;

            elements[2][0] = 0;
            elements[2][1] = 0;
            elements[2][2] = 1;

            break;
        }
    }
};

typedef SquareMatrix<float, 4> Matrix4f;