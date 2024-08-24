#include "../include/Matrix.h"

template <typename T, unsigned int N>
SquareMatrix<T, N>::SquareMatrix(T base_val)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (i == N - 1 && j == i)
                elements[i][j] = 1;
            else if (j == i)
                elements[i][j] = base_val;
            else
                elements[i][j] = 0;
        }
    }
}

template <typename T, unsigned int N>
void SquareMatrix<T, N>::Scale(const Vector3f &Vec)
{
    if (Vec.Size > Size)
        return;

    elements[0][0] *= Vec.x;
    elements[1][1] *= Vec.y;
    elements[2][2] *= Vec.z;
}

template <typename T, unsigned int N>
void SquareMatrix<T, N>::Scale(const Vector2f &Vec)
{
    if (Vec.Size > Size)
        return;

    elements[0][0] *= Vec.x;
    elements[1][1] *= Vec.y;
}

template <typename T, unsigned int N>
void SquareMatrix<T, N>::Translate(const Vector3f &Vec)
{
    if (Vec.Size > Size)
        return;

    elements[0][Size - 1] = Vec.x;

    if (Size > 0)
        elements[1][Size - 1] = Vec.y;

    if (Size > 1)
        elements[2][Size - 1] = Vec.z;

    ConvertToColumnMajorOrder();
}

template <typename T, unsigned int N>
void SquareMatrix<T, N>::Print()
{
    for (int i = 0; i < Size; i++)
    {
        for (int j = 0; j < Size; j++)
        {
            std::cout << elements[i][j];
            std::cout << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

template <typename T, unsigned int N>
void SquareMatrix<T, N>::ConvertToColumnMajorOrder()
{
    T elementsCopy[Size][Size];

    for (int i = 0; i < Size; i++)
        for (int j = 0; j < Size; j++)
            elementsCopy[i][j] = elements[j][i];

    for (int i = 0; i < Size; i++)
        for (int j = 0; j < Size; j++)
            elements[i][j] = elementsCopy[i][j];
}

template <typename T, unsigned int N>
Vector3f SquareMatrix<T, N>::ExtractTranslation() const
{
    if (Size < 4)
        return Vector3f(0, 0, 0);

    return Vector3f(elements[3][0], elements[3][1], elements[3][2]);
}

template <typename T, unsigned int N>
void SquareMatrix<T, N>::Rotate(double Degrees, Axis RotationAxis)
{
    if (Size < 3)
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

template <typename T, unsigned int N>
bool SquareMatrix<T, N>::operator<(SquareMatrix const &b) const
{
    return (elements[0][0] <= b.elements[0][0]);
}

template <typename T, unsigned int N>
Vector4f SquareMatrix<T, N>::operator*(const Vector4f &Vec)
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

template <typename T, unsigned int N>
SquareMatrix<T, N> SquareMatrix<T, N>::operator*(const SquareMatrix<T, N> &MatrixB)
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

template struct SquareMatrix<float, 2>;
template struct SquareMatrix<float, 3>;
template struct SquareMatrix<float, 4>;