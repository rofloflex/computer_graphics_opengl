#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string Shader::loadFile(const char* path)
{
    std::ifstream file(path, std::ios::in);
    if (!file.is_open())
    {
        std::cerr << "ERROR: cannot open shader file: " << path << "\n";
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    ID = 0;

    std::string vSrc = loadFile(vertexPath);
    std::string fSrc = loadFile(fragmentPath);

    if (vSrc.empty() || fSrc.empty())
    {
        std::cerr << "ERROR: shader source is empty. Check paths.\n";
        return;
    }

    const char* vCode = vSrc.c_str();
    const char* fCode = fSrc.c_str();

    // compile vertex
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vCode, nullptr);
    glCompileShader(vs);

    GLint ok = 0;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[1024];
        glGetShaderInfoLog(vs, 1024, nullptr, log);
        std::cerr << "ERROR: vertex shader compile failed:\n" << log << "\n";
        glDeleteShader(vs);
        return;
    }

    // compile fragment
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fCode, nullptr);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[1024];
        glGetShaderInfoLog(fs, 1024, nullptr, log);
        std::cerr << "ERROR: fragment shader compile failed:\n" << log << "\n";
        glDeleteShader(vs);
        glDeleteShader(fs);
        return;
    }

    // link program
    ID = glCreateProgram();
    glAttachShader(ID, vs);
    glAttachShader(ID, fs);
    glLinkProgram(ID);

    glDeleteShader(vs);
    glDeleteShader(fs);

    glGetProgramiv(ID, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[1024];
        glGetProgramInfoLog(ID, 1024, nullptr, log);
        std::cerr << "ERROR: shader program link failed:\n" << log << "\n";
        glDeleteProgram(ID);
        ID = 0;
        return;
    }
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setFloat(const std::string& name, float value) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loc, value);
}