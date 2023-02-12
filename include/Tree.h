#include "Cube.h"
#include "Matrix.h"

#pragma once

class Tree : public Mesh
{
private:
    Cube TrunkCube;
    Cube LeafCube;

    std::vector<Matrix4f> PositionsList;
    std::vector<Matrix4f> LeavesPositionList;

    Vector3f Offset;

public:
    Tree(Vector3f Offset);
    void Draw(Shader *MeshShader, bool isDepth) const;
    void CreateMesh();
};