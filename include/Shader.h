#pragma once

#include <vector>

#include "Matrix.h"

class Shader
{
private:
    unsigned int ProgramId;
    void CheckCompileErrors(GLuint shader, GLenum Type);
    void CheckLinkingErrors(GLuint program);

protected:
    std::vector<unsigned int> shaderIdentifiers;

public:
    Shader(const std::string &FileName);
    Shader(const std::string &FileName, bool HasGeometry);
    void AddShader(const std::string &filepath, GLenum shaderType);
    void LinkShader();

    void SetMatrix4f(const std::string &name, const float *Value) const;
    void SetVector3f(const std::string &name, Vector3f *Vec) const;
    void SetVector4f(const std::string &name, Vector4f *Vec) const;
    void SetFloat(const std::string &name, float value);
    void SetInt(const std::string &name, int value);

    void Use();
};