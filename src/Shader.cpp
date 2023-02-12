#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "../include/Matrix.h"

#include "../include/Shader.h"

Shader::Shader(const std::string &FileName)
{
    auto Filepath1 = std::string("shaders/").append(FileName).append(std::string(".vs")).c_str();
    auto Filepath2 = std::string("shaders/").append(FileName).append(std::string(".fs")).c_str();

    ProgramId = glCreateProgram();

    std::cout << "LOADING: " << FileName << std::endl;

    AddShader(Filepath1, GL_VERTEX_SHADER);
    AddShader(Filepath2, GL_FRAGMENT_SHADER);
    LinkShader();
}

void Shader::CheckCompileErrors(GLuint shader, GLenum Type)
{
    GLint success;
    GLchar infoLog[1024];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);

        if (Type == GL_VERTEX_SHADER)
        {
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
                      << "Vertex"
                      << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
        else
        {
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
                      << "Fragment"
                      << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }

        std::exit(0);
    }
}

void Shader::CheckLinkingErrors(GLuint program)
{
    GLint success;
    GLchar infoLog[1024];

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: "
                  << "\n"
                  << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;

        std::exit(0);
    }
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

    CheckCompileErrors(shaderNumber, shaderType);

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
        glAttachShader(ProgramId, value); // Attaches compilled shader to shader program

    glLinkProgram(ProgramId);

    CheckLinkingErrors(ProgramId);

    // Delete each shader since unused
    for (unsigned int value : shaderIdentifiers)
        glDeleteShader(value); // Cleares memory (free's memory)
}

void Shader::SetMatrix4f(const std::string &name, const float *Value) const
{
    int result = glGetUniformLocation(ProgramId, name.c_str());
    if (result < 0)
        std::cout << "Failed to create uniform matrix " << name.c_str() << std::endl;
    glUniformMatrix4fv(result, 1, GL_FALSE, Value);
}

void Shader::SetVector3f(const std::string &name, Vector3f *Vec) const
{
    int result = glGetUniformLocation(ProgramId, name.c_str());
    if (result < 0)
        std::cout << "Failed to create uniform vec3 " << name.c_str() << std::endl;
    glUniform3f(result, Vec->x, Vec->y, Vec->z);
}

void Shader::SetFloat(const std::string &name, float value)
{
    int result = glGetUniformLocation(ProgramId, name.c_str());
    if (result < 0)
        std::cout << "Failed to create uniform float " << name.c_str() << std::endl;
    glUniform1f(result, value);
}

void Shader::SetInt(const std::string &name, int value)
{
    int result = glGetUniformLocation(ProgramId, name.c_str());
    if (result < 0)
        std::cout << "Failed to create uniform int " << name.c_str() << std::endl;
    glUniform1i(result, value);
}

void Shader::Use()
{
    glUseProgram(ProgramId);
}