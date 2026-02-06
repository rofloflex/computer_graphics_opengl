#pragma once
#include <string>
#include <GL/glew.h>

class Shader
{
public:
    GLuint ID;

    // Загружает шейдеры из файлов
    Shader(const char* vertexPath, const char* fragmentPath);

    // Включить шейдер
    void use() const;

    // Установка uniform
    void setFloat(const std::string& name, float value) const;

private:
    std::string loadFile(const char* path);
};