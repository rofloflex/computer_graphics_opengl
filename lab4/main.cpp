#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// -------------------- настройки окна --------------------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// -------------------- камера --------------------
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// -------------------- callback мыши --------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

// -------------------- изменение окна --------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// -------------------- чтение файла --------------------
std::string readFile(const char* path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Не удалось открыть файл: " << path << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// -------------------- компиляция шейдера --------------------
unsigned int compileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Ошибка компиляции шейдера:\n" << infoLog << "\n";
    }

    return shader;
}

// -------------------- создание shader program --------------------
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vShaderCode);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Ошибка линковки программы:\n" << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n";
        system("pause");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab4 Cube Camera", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        system("pause");
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        glfwTerminate();
        system("pause");
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram = createShaderProgram("vertex.glsl", "fragment.glsl");

    // Куб: координаты + цвет
    float vertices[] = {
        // positions          // colors
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.5f, 1.0f, 0.5f,
         0.5f,  0.5f,  0.5f,  0.5f, 1.0f, 0.5f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.2f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,  0.2f, 0.8f, 0.3f,
        -0.5f,  0.5f, -0.5f,  0.3f, 0.2f, 0.9f,
        -0.5f, -0.5f, -0.5f,  0.9f, 0.3f, 0.2f,
        -0.5f, -0.5f, -0.5f,  0.9f, 0.3f, 0.2f,
        -0.5f, -0.5f,  0.5f,  0.2f, 0.7f, 0.8f,
        -0.5f,  0.5f,  0.5f,  0.2f, 0.8f, 0.3f,

         0.5f,  0.5f,  0.5f,  0.8f, 0.1f, 0.6f,
         0.5f,  0.5f, -0.5f,  0.1f, 0.9f, 0.4f,
         0.5f, -0.5f, -0.5f,  0.6f, 0.6f, 0.1f,
         0.5f, -0.5f, -0.5f,  0.6f, 0.6f, 0.1f,
         0.5f, -0.5f,  0.5f,  0.1f, 0.5f, 0.9f,
         0.5f,  0.5f,  0.5f,  0.8f, 0.1f, 0.6f,

        -0.5f, -0.5f, -0.5f,  0.9f, 0.4f, 0.4f,
         0.5f, -0.5f, -0.5f,  0.4f, 0.9f, 0.4f,
         0.5f, -0.5f,  0.5f,  0.4f, 0.4f, 0.9f,
         0.5f, -0.5f,  0.5f,  0.4f, 0.4f, 0.9f,
        -0.5f, -0.5f,  0.5f,  0.9f, 0.9f, 0.4f,
        -0.5f, -0.5f, -0.5f,  0.9f, 0.4f, 0.4f,

        -0.5f,  0.5f, -0.5f,  0.4f, 0.9f, 0.9f,
         0.5f,  0.5f, -0.5f,  0.9f, 0.4f, 0.9f,
         0.5f,  0.5f,  0.5f,  0.8f, 0.8f, 0.8f,
         0.5f,  0.5f,  0.5f,  0.8f, 0.8f, 0.8f,
        -0.5f,  0.5f,  0.5f,  0.3f, 0.7f, 0.6f,
        -0.5f,  0.5f, -0.5f,  0.4f, 0.9f, 0.9f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::cout << "WASD + mouse camera enabled\n";

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        const float cameraSpeed = 0.05f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::mat4(1.0f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            100.0f
        );

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}