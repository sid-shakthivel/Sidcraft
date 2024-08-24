#pragma once

#include <iostream>

template <typename T, unsigned int N>
struct Vector
{
    // Static member to store the dimensionality of the vector (2 or 3).
    static constexpr auto Size = N;

    Vector();
    Vector(T _x, T _y);
    Vector(T _x, T _y, T _z);
    Vector(T _x, T _y, T _z, T _w);

    float Magnitude();
    bool IsEqual(const Vector &VecB) const;
    void Negate();
    T DotProduct(const Vector &VecB) const;
    Vector CrossProduct(Vector &VecB);
    Vector Normalise();
    void RoundToNearestInt();
    void Print();

    Vector operator+(const Vector &VecB);

    Vector operator-(const Vector &VecB);

    Vector operator*(const Vector &VecB);
    Vector operator*(T Scalar);

    Vector operator/(T scalar);

    bool operator<(Vector const &b) const;

    T x;
    T y;
    T z;
    T w;
};

typedef Vector<float, 4> Vector4f;
typedef Vector<float, 3> Vector3f;
typedef Vector<float, 2> Vector2f;