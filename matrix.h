#pragma once

#include <math.h>
#include <iostream>

enum Axis
{
    X_AXIS,
    Y_AXIS,
    Z_AXIS
};

template <class T, unsigned int N>
class Vector
{
private:
public:
    static constexpr auto Size = N;

    T x;
    T y;
    T z;

    Vector(T _x, T _y)
    {
        x = _x;
        y = _y;
        z = 0;
    }

    Vector(T _x, T _y, T _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector()
    {
        x = 0, y = 0, z = 0;
    }

    float GetMagnitude()
    {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    Vector Add(Vector VecB)
    {
        return Vector(x + VecB.x, y + VecB.y, z + VecB.z);
    }

    void NegateVector()
    {
        x = -x;
        y = -y;
        z = -z;
    }

    void Add(T Scalar)
    {
        x, y, z += Scalar;
    }

    Vector Sub(Vector VecB)
    {
        return Vector(x - VecB.x, y - VecB.y, z - VecB.z);
    }

    void Sub(T Scalar)
    {
        x, y, z -= Scalar;
    }

    T DotProduct()
    {
    }

    Vector CrossProduct(Vector VecA, Vector VecB)
    {
        T CrossX = (VecA.y * VecB.z) - (VecA.z * VecB.y);
        T CrossY = (VecA.z * VecB.x) - (VecA.x * VecB.z);
        T CrossZ = (VecA.x * VecB.y) - (VecA.y * VecB.x);

        return Vector(CrossX, CrossY, CrossZ);
    }

    void Normalise()
    {
        float magnitude = GetMagnitude();

        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
    }

    void Print()
    {
        std::cout << "Vector: " << x << " " << y << " " << z << "\n\n";
    }

    Vector Multiply(T Scalar)
    {
        return Vector(x * Scalar, y * Scalar, z * Scalar);
    }

    Vector Mutiply(Vector VecB)
    {
        return Vector(x * VecB.x, y * VecB.y, z * VecB.z);
    }
};

typedef Vector<float, 3> Vector3f;
typedef Vector<float, 2> Vector2f;

template <class T, unsigned int N>
class SquareMatrix
{
private:
    static constexpr auto Size = N;

public:
    T elements[N][N];

    SquareMatrix(T base_val)
    {
        for (int i = 0; i < Size; i++)
        {
            for (int j = 0; j < Size; j++)
            {
                if (i == Size - 1 && j == i)
                    elements[i][j] = 1;
                else if (j == i)
                    elements[i][j] = base_val;
                else
                    elements[i][j] = 0;
            }
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
        T elementsCopy[Size][Size];

        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
            {
                elementsCopy[i][j] = 0;
                for (int k = 0; k < Size; k++)
                    elementsCopy[i][j] += elements[i][k] * MatrixB.elements[k][j];
            }

        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
                elements[i][j] = elementsCopy[i][j];
    }

    void Multiply(T Scalar)
    {
        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
                elements[i][j] *= Scalar;
    }

    // Scale a matrix by a vector
    void Scale(Vector3f Vec)
    {
        elements[0][0] *= Vec.x;
        elements[1][1] *= Vec.y;
        elements[2][2] *= Vec.z;
    }

    void Scale(Vector2f Vec)
    {
        if (Vec.Size != Size)
            return;

        elements[0][0] *= Vec.x;
        elements[1][1] *= Vec.y;
    }

    // Creates a transformation matrix (opengl style) in which last column gets set to a vector
    void Translate(Vector3f Vec)
    {
        // if (Vec.Size > Size)
        //     return;

        elements[0][Size - 1] = Vec.x;

        if (Size > 0)
            elements[1][Size - 1] = Vec.y;

        if (Size > 1)
            elements[2][Size - 1] = Vec.z;

        ConvertToColumnMajorOrder();
    }

    void Print()
    {
        for (int i = 0; i < Size; i++)
        {
            for (int j = 0; j < Size; j++)
            {
                std::cout << elements[i][j];
                std::cout << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    void ConvertToColumnMajorOrder()
    {
        T elementsCopy[Size][Size];

        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
                elementsCopy[i][j] = elements[j][i];

        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
                elements[i][j] = elementsCopy[i][j];
    }

    // For 3D rotations
    void Rotate(double Degrees, Vector3f Direction)
    {
        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
                elements[i][j] = 0;

        // X Axis
        elements[0][0] += 1 * Direction.x;
        elements[0][1] += 0 * Direction.x;
        elements[0][2] += 0 * Direction.x;

        elements[1][0] += 0 * Direction.x;
        elements[1][1] += cos(Degrees) * Direction.x;
        elements[1][2] += -sin(Degrees) * Direction.x;

        elements[2][0] += 0 * Direction.x;
        elements[2][1] += sin(Degrees) * Direction.x;
        elements[2][2] += cos(Degrees) * Direction.x;

        // Y Axis
        elements[0][0] += cos(Degrees) * Direction.y;
        elements[0][1] += 0 * Direction.y;
        elements[0][2] += sin(Degrees) * Direction.y;

        elements[1][0] += 0 * Direction.y;
        elements[1][1] += 1 * Direction.y;
        elements[1][2] += 0 * Direction.y;

        elements[2][0] += -sin(Degrees) * Direction.y;
        elements[2][1] += 0 * Direction.y;
        elements[2][2] += cos(Degrees) * Direction.y;

        // Z Axis
        elements[0][0] += cos(Degrees) * Direction.z;
        elements[0][1] += -sin(Degrees) * Direction.z;
        elements[0][2] += 0 * Direction.z;

        elements[1][0] += sin(Degrees) * Direction.z;
        elements[1][1] += cos(Degrees) * Direction.z;
        elements[1][2] += 0 * Direction.z;

        elements[2][0] += 0 * Direction.z;
        elements[2][1] += 0 * Direction.z;
        elements[2][2] += 1 * Direction.z;
    }
};

typedef SquareMatrix<float, 4> Matrix4f;
typedef SquareMatrix<float, 3> Matrix3f;
typedef SquareMatrix<float, 2> Matrix2f;

Matrix4f CreatePerspectiveProjectionMatrix(float Fov, float Aspect, float Near, float Far)
{
    Matrix4f ProjectionMatrix = Matrix4f(0);

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            ProjectionMatrix.elements[i][j] = 0;

    ProjectionMatrix.elements[0][0] = 1.0f / tan(Fov / 2.0f) / Aspect;
    ProjectionMatrix.elements[1][1] = 1.0f / tan(Fov / 2.0f);
    ProjectionMatrix.elements[2][2] = (Far + Near) / (Near - Far);
    ProjectionMatrix.elements[2][3] = -1.0f;
    ProjectionMatrix.elements[3][2] = (2 * Far * Near) / (Near - Far);

    return ProjectionMatrix;
}

Matrix4f CreateLookAtMatrix(Vector3f PositionVector, Vector3f TargetVector, Vector3f UpVector)
{
    Matrix4f MatrixA = Matrix4f(1);

    Vector3f DirectionVector = TargetVector.Sub(PositionVector);
    DirectionVector.Normalise();

    Vector3f RightVector = DirectionVector.CrossProduct(UpVector, DirectionVector);
    RightVector.Normalise();

    Vector3f RelativeUpVector = RightVector.CrossProduct(DirectionVector, RightVector);

    MatrixA.elements[0][0] = RightVector.x;
    MatrixA.elements[0][1] = RelativeUpVector.x;
    MatrixA.elements[0][2] = DirectionVector.x;
    MatrixA.elements[0][3] = 0;

    MatrixA.elements[1][0] = RightVector.y;
    MatrixA.elements[1][1] = RelativeUpVector.y;
    MatrixA.elements[1][2] = DirectionVector.y;
    MatrixA.elements[1][3] = 0;

    MatrixA.elements[2][0] = RightVector.z;
    MatrixA.elements[2][1] = RelativeUpVector.z;
    MatrixA.elements[2][2] = DirectionVector.z;
    MatrixA.elements[2][3] = 0;

    MatrixA.elements[3][0] = 0;
    MatrixA.elements[3][1] = 0;
    MatrixA.elements[3][2] = 0;
    MatrixA.elements[3][3] = 1;

    Matrix4f MatrixB = Matrix4f(1);
    PositionVector.NegateVector();
    MatrixB.Translate(PositionVector);

    MatrixA.Multiply(MatrixB);

    return MatrixA;
}