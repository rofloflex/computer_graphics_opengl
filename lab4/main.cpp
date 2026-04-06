#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Model.h"

using namespace std;

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ѕараметры подвижности
float offsetZ = 0.0f;   // Cube.001 Ч движение по Z
float angleY = 0.0f;   // Cube.003 Ч поворот вокруг Y
float offsetX = 0.0f;   // Cube.002 Ч движение по X
float offsetY = 0.0f;   // Cylinder.001 Ч движение по Y

string readFile(const char* filepath)
{
    ifstream file(filepath);
    if (!file.is_open())
    {
        cout << "ERROR::FILE_NOT_OPEN " << filepath << endl;
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    string vertexCode = readFile(vertexPath);
    string fragmentCode = readFile(fragmentPath);

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    GLint success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::VERTEX_SHADER\n" << infoLog << endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::FRAGMENT_SHADER\n" << infoLog << endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER_PROGRAM\n" << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float moveStep = deltaTime * 0.8f;
    float rotateStep = deltaTime * 50.0f;

    // Cube.001 Ч движение по Z (Q / A)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        float newVal = offsetZ + moveStep;
        if (newVal <= 0.7f)
            offsetZ = newVal;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        float newVal = offsetZ - moveStep;
        if (newVal >= -0.7f)
            offsetZ = newVal;
    }

    // Cube.003 Ч поворот вокруг Y (W / S)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        float newVal = angleY + rotateStep;
        if (newVal <= 60.0f)
            angleY = newVal;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        float newVal = angleY - rotateStep;
        if (newVal >= -60.0f)
            angleY = newVal;
    }

    // Cube.002 Ч движение по X (E / D)
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        float newVal = offsetX + moveStep;
        if (newVal <= 0.1f)
            offsetX = newVal;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        float newVal = offsetX - moveStep;
        if (newVal >= -0.5f)
            offsetX = newVal;
    }

    // Cylinder.001 Ч движение по Y (R / F)
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        float newVal = offsetY + moveStep;
        if (newVal <= 0.05f)
            offsetY = newVal;
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        float newVal = offsetY - moveStep;
        if (newVal >= -0.3f)
            offsetY = newVal;
    }
}

int main()
{
    if (!glfwInit())
    {
        cout << "ERROR::GLFW_INIT_FAILED" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 7 - Affine Transformations", NULL, NULL);
    if (window == NULL)
    {
        cout << "ERROR::WINDOW_CREATION_FAILED" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "ERROR::GLAD_INIT_FAILED" << endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    GLuint shaderProgram = createShaderProgram("vertex.glsl", "fragment.glsl");

    Model myModel("models/electronica_V3.obj");

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 6.0f);

        glm::mat4 view = glm::lookAt(
            cameraPos,
            glm::vec3(0.0f, 0.4f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            100.0f
        );

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3f(glGetUniformLocation(shaderProgram, "light.position"), 3.0f, 3.0f, 3.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);

        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.7f, 0.7f, 0.7f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.7f, 0.7f, 0.7f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.9f, 0.9f, 0.9f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);

        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));

        myModel.Draw(shaderProgram, offsetZ, angleY, offsetX, offsetY);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}