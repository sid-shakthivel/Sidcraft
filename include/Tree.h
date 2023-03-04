#pragma once

#include "Cube.h"
#include "Matrix.h"

class TestTree : public Mesh
{
private:
    Cube TrunkCube;
    Cube LeafCube;

    std::vector<Matrix4f> PositionsList;
    std::vector<Matrix4f> LeavesPositionList;

    Vector3f Offset;

public:
    TestTree(Vector3f Offset);
    void DrawTrunk(Shader *MeshShader) const;
    void DrawLeaves(Shader *MeshShader) const;
    void CreateMesh();
};