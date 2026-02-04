#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void drawStar()
{
    float vertices[][2] = {
        { 0.0f,  0.0f},   // 0. Центр звезды (нужен для корректного Triangle Fan)
        { 0.0f,  0.5f},   // 1. Верхняя вершина
        { 0.12f, 0.15f},  // 2. Внутренняя 
        { 0.5f,  0.15f},  // 3. Правая
        { 0.2f, -0.1f},   // 4. Внутренняя
        { 0.35f, -0.5f},  // 5. Нижняя правая
        { 0.0f, -0.25f},  // 6. Внутренняя нижняя
        {-0.35f, -0.5f},  // 7. Нижняя левая
        {-0.2f, -0.1f},   // 8. Внутренняя
        {-0.5f,  0.15f},  // 9. Левая
        {-0.12f, 0.15f},  // 10. Внутренняя
        { 0.0f,  0.5f}    // 11. Замыкающая (та же, что и 1-я вершина луча)
    };

    // Triangle Fan рисует треугольники от центра (вершина 0) к каждой паре точек
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 12; ++i)
    {
        glVertex2f(vertices[i][0], vertices[i][1]);
    }
    glEnd();
}

int main()
{
    if (!glfwInit())
    {
        std::cout << "Ошибка инициализации GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(512, 512, "STAR", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Ошибка создания окна\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Ошибка инициализации GLEW\n";
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GPU: " << glGetString(GL_RENDERER) << "\n";

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // белый
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(0.7f, 1.0f, 0.3f); // лаймовая
        drawStar();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
