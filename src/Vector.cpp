#include "../include/Vector.h"

template <typename T, unsigned int N>
Vector<T, N>::Vector() : x(0), y(0), z(0), w(0) {}

template <typename T, unsigned int N>
Vector<T, N>::Vector(T _x, T _y) : x(_x), y(_y), z(0) {}

template <typename T, unsigned int N>
Vector<T, N>::Vector(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

template <typename T, unsigned int N>
Vector<T, N>::Vector(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}

template <typename T, unsigned int N>
float Vector<T, N>::Magnitude()
{
    return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

template <typename T, unsigned int N>
bool Vector<T, N>::IsEqual(const Vector &VecB) const
{
    return (x == VecB.x && y == VecB.y && z == VecB.z);
}

template <typename T, unsigned int N>
void Vector<T, N>::Negate()
{
    x = -x;
    y = -y;
    z = -z;
}

template <typename T, unsigned int N>
T Vector<T, N>::DotProduct(const Vector &VecB) const
{
    return x * VecB.x + y * VecB.y + z * VecB.z;
}

template <typename T, unsigned int N>
Vector<T, N> Vector<T, N>::CrossProduct(Vector &VecB)
{
    T CrossX = (y * VecB.z) - (z * VecB.y);
    T CrossY = (z * VecB.x) - (x * VecB.z);
    T CrossZ = (x * VecB.y) - (y * VecB.x);

    return Vector(CrossX, CrossY, CrossZ);
}

template <typename T, unsigned int N>
Vector<T, N> Vector<T, N>::Normalise()
{
    float magnitude = Magnitude();
    return Vector(x / magnitude, y / magnitude, z / magnitude);
}

template <typename T, unsigned int N>
void Vector<T, N>::RoundToNearestInt()
{
    x = round(x);
    y = round(y);
    z = round(z);
    w = round(w);
}

template <typename T, unsigned int N>
void Vector<T, N>::Print()
{
    std::cout << "Vector: " << x << ", " << y << ", " << z << std::endl;
}

template <typename T, unsigned int N>
bool Vector<T, N>::operator<(Vector const &b) const
{
    return (x < b.x) && (y < b.y) && (z < b.z);
}

template <typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator+(Vector const &b)
{
    return Vector(x + b.x, y + b.y, z + b.z, w + b.w);
}

template <typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator-(Vector const &b)
{
    return Vector(x - b.x, y - b.y, z - b.z, w - b.w);
}

template <typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator*(Vector const &b)
{
    return Vector(x * b.x, y * b.y, z * b.z, w * b.w);
}

template <typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator*(T Scalar)
{
    return Vector(x * Scalar, y * Scalar, z * Scalar, w * Scalar);
}

template <typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator/(T scalar)
{
    return Vector(x / scalar, y / scalar, z / scalar);
}

template struct Vector<float, 2>;
template struct Vector<float, 3>;
template struct Vector<float, 4>;
