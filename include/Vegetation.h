#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Matrix.h"

class Vegetation : public Mesh
{
private:
public:
    Vegetation(float TextureIndex);
    void Draw(Shader *MeshShader, Matrix4f Offset) const;
    void CreateMesh();
};