#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Matrix.h"

class Vegetation : public Mesh
{
private:
    Matrix4f Position = Matrix4f(1);    

public:
    Vegetation(float TextureIndex, Matrix4f Position);
    void Draw(Shader *MeshShader, bool IsDepth) const;
    void CreateMesh();
};