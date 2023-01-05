#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "../include/Matrix.h"

#include "../include/Shader.h"

Shader::Shader()
{
    programId = glCreateProgram();
}

void Shader::AddShader(const char *filepath, GLenum shaderType)
{
    std::ostringstream sstream;
    std::ifstream fs(filepath);
    sstream << fs.rdbuf();
    const std::string str(sstream.str());
    const char *fileContents = str.c_str();

    unsigned int shaderNumber = glCreateShader(shaderType);
    glShaderSource(shaderNumber, 1, &fileContents, NULL); // Second parameter specifies number of strings which are source code
    glCompileShader(shaderNumber);

    char infoLog[512];
    int success;
    glGetProgramiv(shaderNumber, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderNumber, 512, NULL, infoLog);
    }

    shaderIdentifiers.push_back(shaderNumber);
}

void Shader::LinkShader()
{
    // Attach each shader
    for (unsigned int value : shaderIdentifiers)
        glAttachShader(programId, value); // Attaches compilled shader to shader program

    glLinkProgram(programId);

    // Delete each shader since unused
    for (unsigned int value : shaderIdentifiers)
        glDeleteShader(value); // Cleares memory (free's memory)
}

void Shader::SetMatrix4f(const std::string &name, const float *Value) const
{
    int result = glGetUniformLocation(programId, name.c_str());
    if (result < 0)
        std::cout << "Failed to create uniform matrix\n";
    glUniformMatrix4fv(result, 1, GL_FALSE, Value);
}

void Shader::SetVector3f(const std::string &name, Vector3f *Vec) const
{
    int result = glGetUniformLocation(programId, name.c_str());
    if (result < 0)
        std::cout << "Failed to create uniform vec3\n";
    glUniform3f(result, Vec->x, Vec->y, Vec->z);
}

void Shader::SetFloat(const std::string &name, float value)
{
    int result = glGetUniformLocation(programId, name.c_str());
    if (result < 0)
        std::cout << "Failed to create uniform float\n";
    glUniform1f(result, value);
}

void Shader::SetInt(const std::string &name, int value)
{
    int result = glGetUniformLocation(programId, name.c_str());
    if (result < 0)
        std::cout << "Failed to create uniform int\n";
    glUniform1i(result, value);
}

void Shader::Use()
{
    glUseProgram(programId);
}