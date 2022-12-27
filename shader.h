#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "matrix.h"

class Shader
{
protected:
    std::vector<unsigned int> shaderIdentifiers;

public:
    Shader()
    {
        programId = glCreateProgram();
    }

    unsigned int programId;

    void AddShader(const char *filepath, GLenum shaderType)
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
            // std::cout << infoLog;
            // std::cout << "Error when loading shaders!\n";
        }

        shaderIdentifiers.push_back(shaderNumber);
    }

    void LinkShader()
    {
        // Attach each shader
        for (unsigned int value : shaderIdentifiers)
            glAttachShader(programId, value); // Attaches compilled shader to shader program

        glLinkProgram(programId);

        // Delete each shader since unused
        for (unsigned int value : shaderIdentifiers)
            glDeleteShader(value); // Cleares memory (free's memory)
    }

    void SetMatrix4f(const std::string &name, const float *Value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, Value);
    }

    void SetVector3f(const std::string &name, Vector3f *Vec) const
    {
        glUniform3f(glGetUniformLocation(programId, name.c_str()), Vec->x, Vec->y, Vec->z);
    }

    void Use()
    {
        glUseProgram(programId);
    }
};
