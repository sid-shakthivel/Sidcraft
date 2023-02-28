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
    T w;

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

    Vector(T _x, T _y, T _z, T _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
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

    const bool IsEqual(Vector VecB)
    {
        if (x == VecB.x && y == VecB.y && z == VecB.z)
            return true;
        return false;
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

    T DotProduct(Vector VecA, Vector VecB)
    {
        return VecA.x * VecB.x + VecA.y * VecB.y + VecA.z * VecB.z;
    }

    Vector CrossProduct(Vector VecA, Vector VecB)
    {
        T CrossX = (VecA.y * VecB.z) - (VecA.z * VecB.y);
        T CrossY = (VecA.z * VecB.x) - (VecA.x * VecB.z);
        T CrossZ = (VecA.x * VecB.y) - (VecA.y * VecB.x);

        return Vector(CrossX, CrossY, CrossZ);
    }

    Vector Divide(T Scalar)
    {
        return Vector(x / Scalar, y / Scalar, z / Scalar);
    }

    void Normalise()
    {
        float magnitude = GetMagnitude();

        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
    }

    Vector ReturnNormalise()
    {
        float magnitude = GetMagnitude();
        return Vector(x / magnitude, y / magnitude, z / magnitude);
    }

    void RoundToNearestInt()
    {
        x = round(x);
        y = round(y);
        z = round(z);
    }

    Vector Negate()
    {
        return Vector(-x, -y, -z);
    }

    void Print()
    {
        std::cout << "Vector: " << x << " " << y << " " << z << "\n";
    }

    Vector Multiply(T Scalar)
    {
        return Vector(x * Scalar, y * Scalar, z * Scalar);
    }

    Vector Mutiply(Vector VecB)
    {
        return Vector(x * VecB.x, y * VecB.y, z * VecB.z);
    }

    bool operator<(Vector const &b) const
    { // <-- note the *trailing* const!
        return (x < b.x) && (y < b.y) && (z < b.z);
    }
};

typedef Vector<float, 4> Vector4f;
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

    SquareMatrix Multiply(SquareMatrix MatrixB)
    {
        SquareMatrix<T, Size> NewMatrix = SquareMatrix<T, Size>(1);

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
                NewMatrix.elements[i][j] = elementsCopy[i][j];

        return NewMatrix;
    }

    Vector4f Multiply(Vector4f Vec)
    {
        if (Size < 4)
        {
            std::cout << "ERROR: ATTEMPTING TO MUTLIPLY VEC4 BY MATRIX" << std::endl;
            std::exit(0);
        }

        auto X = Vec.x * elements[0][0] + Vec.y * elements[1][0] + Vec.z * elements[2][0] + Vec.w * elements[3][0];
        auto Y = Vec.x * elements[0][1] + Vec.y * elements[1][1] + Vec.z * elements[2][1] + Vec.w * elements[3][1];
        auto Z = Vec.x * elements[0][2] + Vec.y * elements[1][2] + Vec.z * elements[2][2] + Vec.w * elements[3][2];
        auto W = Vec.x * elements[0][3] + Vec.y * elements[1][3] + Vec.z * elements[2][3] + Vec.w * elements[3][3];

        return Vector4f(X, Y, Z, W);
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

    bool operator<(SquareMatrix const &b) const
    { // <-- note the *trailing* const!
        return (elements[0][0] <= b.elements[0][0]);
    }

    Vector3f ExtractTranslation() const
    {
        return Vector3f(elements[3][0], elements[3][1], elements[3][2]);
    }

    void Rotate(double Degrees, Axis RotationAxis)
    {
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
typedef SquareMatrix<float, 3> Matrix3f;
typedef SquareMatrix<float, 2> Matrix2f;

inline Matrix4f CreatePerspectiveProjectionMatrix(float Fov, float Aspect, float Near, float Far)
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

inline Matrix4f CreateOrthographicProjectionMatrix(float Bottom, float Top, float Left, float Right, float Near, float Far)
{
    Matrix4f MatrixA = Matrix4f(1);

    MatrixA.elements[0][0] = 2 / (Right - Left);
    MatrixA.elements[0][1] = 0;
    MatrixA.elements[0][2] = 0;
    MatrixA.elements[0][3] = 0;

    MatrixA.elements[1][0] = 0;
    MatrixA.elements[1][1] = 2 / (Top - Bottom);
    MatrixA.elements[1][2] = 0;
    MatrixA.elements[1][3] = 0;

    MatrixA.elements[2][0] = 0;
    MatrixA.elements[2][1] = 0;
    MatrixA.elements[2][2] = -2 / (Far - Near);
    MatrixA.elements[2][3] = 0;

    MatrixA.elements[3][0] = -(Right + Left) / (Right - Left);
    MatrixA.elements[3][1] = -(Top + Bottom) / (Top - Bottom);
    MatrixA.elements[3][2] = -(Far + Near) / (Far - Near);
    MatrixA.elements[3][3] = 1;

    return MatrixA;
}

inline Matrix4f CreateLookAtMatrix(Vector3f PositionVector, Vector3f TargetVector, Vector3f UpVector)
{
    Matrix4f MatrixA = Matrix4f(1);

    Vector3f ZAxis = TargetVector.Sub(PositionVector).ReturnNormalise();
    Vector3f XAxis = ZAxis.CrossProduct(ZAxis, UpVector).ReturnNormalise();
    Vector3f YAxis = XAxis.CrossProduct(XAxis, ZAxis);

    ZAxis = ZAxis.Negate();

    MatrixA.elements[0][0] = XAxis.x;
    MatrixA.elements[0][1] = XAxis.y;
    MatrixA.elements[0][2] = XAxis.z;
    MatrixA.elements[0][3] = -XAxis.DotProduct(XAxis, PositionVector);

    MatrixA.elements[1][0] = YAxis.x;
    MatrixA.elements[1][1] = YAxis.y;
    MatrixA.elements[1][2] = YAxis.z;
    MatrixA.elements[1][3] = -YAxis.DotProduct(YAxis, PositionVector);

    MatrixA.elements[2][0] = ZAxis.x;
    MatrixA.elements[2][1] = ZAxis.y;
    MatrixA.elements[2][2] = ZAxis.z;
    MatrixA.elements[2][3] = -ZAxis.DotProduct(ZAxis, PositionVector);

    MatrixA.elements[3][0] = 0;
    MatrixA.elements[3][1] = 0;
    MatrixA.elements[3][2] = 0;
    MatrixA.elements[3][3] = 1;

    MatrixA.ConvertToColumnMajorOrder();

    return MatrixA;
}
