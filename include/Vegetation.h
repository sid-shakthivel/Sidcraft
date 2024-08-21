#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Matrix.h"

class Vegetation
{
private:
    Matrix4f Position = Matrix4f(1);

public:
    Vegetation(float TextureIndex, Vector3f TranslationVector);
    void Draw(Shader *MeshShader);
    void CreateMesh();
};