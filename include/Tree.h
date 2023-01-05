#include "Cube.h"
#include "Matrix.h"

#pragma once

class Tree : public Mesh
{
private:
    Cube cube = Cube();
    MeshData CubeData;
    std::vector<Matrix4f> PositionsList;
    std::vector<Matrix4f> LeavesPositionList;

public:
    Tree(Vector3f Offset);
    void Draw(Shader *MeshShader, bool isDepth);
    void CreateMesh(Vector3f Offset);
};
