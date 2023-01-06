#pragma once

#include <vector>

#include "Matrix.h"

class Shader
{
private:
    unsigned int programId;

protected:
    std::vector<unsigned int> shaderIdentifiers;

public:
    Shader();
    void AddShader(const char *filepath, GLenum shaderType);
    void LinkShader();

    void SetMatrix4f(const std::string &name, const float *Value) const;
    void SetVector3f(const std::string &name, Vector3f *Vec) const;
    void SetFloat(const std::string &name, float value);
    void SetInt(const std::string &name, int value);

    void Use();
};