#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class Shader
{
protected:
    Shader()
    {
        programId = glCreateProgram();
    }

    std::vector<unsigned int> shaderIdentifiers;

public:
    unsigned int programId;

    Shader(Shader &other) = delete;          // Singletons should not be cloneable
    void operator=(const Shader &) = delete; // Singletons should not be assignabl

    static Shader *GetInstance();

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
            std::cout << "Error when loading shaders!\n";
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

    void SetBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(programId, name.c_str()), (int)value);
    }

    void SetInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
    }

    void SetFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
    }

    void Use()
    {
        glUseProgram(programId);
    }
};

Shader *singleton_ = nullptr;

// Static methods are defined outside
Shader *Shader::GetInstance()
{
    if (singleton_ == nullptr)
    {
        singleton_ = new Shader();
    }
    return singleton_;
}