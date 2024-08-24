#pragma once

#include "./Vector.h"

#include <math.h>
#include <iostream>

enum Axis
{
    X_AXIS,
    Y_AXIS,
    Z_AXIS
};

template <typename T, unsigned int N>
struct SquareMatrix
{
private:
    static constexpr auto Size = N;

public:
    T elements[N][N];

    SquareMatrix(T base_val);

    SquareMatrix operator*(SquareMatrix const &MatrixB);
    Vector4f operator*(const Vector4f &Vec);

    void Scale(const Vector3f &Vec); // Scale a matrix by a vector
    void Scale(const Vector2f &Vec);

    void Translate(const Vector3f &Vec); // Creates a transformation matrix (opengl style) in which last column gets set to a vector
    void Print();

    void ConvertToColumnMajorOrder();

    bool operator<(SquareMatrix const &b) const;

    Vector3f ExtractTranslation() const;

    void Rotate(double Degrees, Axis RotationAxis);
};

typedef SquareMatrix<float, 4> Matrix4f;
typedef SquareMatrix<float, 3> Matrix3f;
typedef SquareMatrix<float, 2> Matrix2f;
