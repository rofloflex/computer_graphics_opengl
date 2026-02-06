#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include <iostream>
#include <vector>
#include <cmath>

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    if (!glfwInit())
    {
        std::cout << "Ошибка инициализации GLFW\n";
        return -1;
    }

    // OpenGL 4.6 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(512, 512, "STAR (VAO/VBO/EBO)", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Ошибка создания окна\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Ошибка инициализации GLEW\n";
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GPU: " << glGetString(GL_RENDERER) << "\n";

    // Формат:x,y
    float vertices[] = {
        0.0f,  0.0f,
        0.0f,  0.5f,
        0.12f, 0.15f,
        0.5f,  0.15f,
        0.2f, -0.1f,
        0.35f,-0.5f,
        0.0f, -0.25f,
       -0.35f,-0.5f,
       -0.2f, -0.1f,
       -0.5f,  0.15f,
       -0.12f, 0.15f
    };

    // Индексы для Triangle Fan:
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 7,
        0, 7, 8,
        0, 8, 9,
        0, 9, 10,
        0, 10, 1
    };
    const unsigned int indexCount = sizeof(indices) / sizeof(indices[0]);

    // VAO/VBO/EBO
    GLuint VAO = 0, VBO = 0, EBO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // layout(location=0) to vec2 position
    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE,
        2 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    Shader shader("shaders/star.vert", "shaders/star.frag");
    if (shader.ID == 0)
    {
        glfwTerminate();
        return -1;
    }

    // Рендер-цикл
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        float t = (float)glfwGetTime();
        shader.setFloat("uTime", t);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // очистка ресурсов
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    return 0;
}