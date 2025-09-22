#include <Windows.h>

#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct Object {
    GLuint program;
    GLuint vao;
    GLsizei count;
};

std::string readFile(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    std::string content;
    content.assign(std::istreambuf_iterator<char>(file),
                   std::istreambuf_iterator<char>()
    );

    return content;
}

Object initObj() {
    Object obj{};

    // Create and compile vertex shader
    const std::string vertexString = readFile("shader/shader.vert");
    const GLchar *vertexText = vertexString.c_str();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexText, nullptr);
    glCompileShader(vertexShader);
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(vertexShader, 1024, nullptr, infoLog);
        std::cout << "Error compiling vertex shader: " << infoLog << std::endl;
    }

    // Create and compile fragment shader
    const std::string fragmentString = readFile("shader/shader.frag");
    const GLchar *fragmentText = fragmentString.c_str();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentText, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(fragmentShader, 1024, nullptr, infoLog);
        std::cout << "Error compiling fragment shader: " << infoLog << std::endl;
    }

    // Create and link shader program
    obj.program = glCreateProgram();
    glAttachShader(obj.program, vertexShader);
    glAttachShader(obj.program, fragmentShader);
    glLinkProgram(obj.program);
    glGetProgramiv(obj.program, GL_LINK_STATUS, &status);
    if (!status) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(obj.program, 1024, nullptr, infoLog);
        std::cout << "Error linking program: " << infoLog << std::endl;
    }
    glValidateProgram(obj.program);
    glGetProgramiv(obj.program, GL_VALIDATE_STATUS, &status);
    if (!status) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(obj.program, 1024, nullptr, infoLog);
        std::cout << "Error validating program: " << infoLog << std::endl;
    }


    struct Position {
        GLfloat x;
        GLfloat y;
    };

    struct Vertex {
        glm::vec2 position;
    };

    // Triangle vertices
    std::vector<Vertex> vertices = {
        {{0.5f * 0.5f, 0.5f * 0.5f}},
        {{0.5f * 0.5f, -0.5f * 0.5f}},
        {{-0.5f * 0.5f, -0.5f * 0.5f}},
    };
    obj.count = vertices.size();

    // Create vertex buffer object
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create vertex array object
    glGenVertexArrays(1, &(obj.vao));
    glBindVertexArray(obj.vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        offsetof(Vertex, position)
    );
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return obj;
}

// Function to generate a color that changes over time
glm::vec3 getColor(float time) {
    // Use sine function to create a smooth color transition over time
    float r = (sin(time * 2.0f) + 1.0f) / 2.0f; // Cycle from 0.0 to 1.0
    float g = (sin(time * 2.0f + 2.0f) + 1.0f) / 2.0f; // Cycle from 0.0 to 1.0
    float b = (sin(time * 2.0f + 4.0f) + 1.0f) / 2.0f; // Cycle from 0.0 to 1.0

    return glm::vec3(r, g, b);
}

glm::vec2 getRandomOffset(float time) {
    // Generate random values based on time
    float x = (sin(-time * 1.5f) * 0.5f + 1.0f) * 0.5f * 2.0f - 1.0f; // Value between -1.0 and 1.0
    float y = (cos(time * 2.5f) * 0.5f + 1.0f) * 0.5f * 2.0f - 1.0f; // Value between -1.0 and 1.0

    return glm::vec2(x, y); // Return as vec2 offset
}


void draw(Object obj) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(obj.program);

    // Get current time and update the color
    float time = (float) glfwGetTime();
    glm::vec3 color = getColor(time); // Get the dynamic color

    // Set the color uniform
    GLuint colorLoc = glGetUniformLocation(obj.program, "color");
    glUniform3f(colorLoc, color.r, color.g, color.b);


    glBindVertexArray(obj.vao);
    glDrawArrays(GL_TRIANGLES, 0, obj.count);
}

void framebuffer_size_callback(GLFWwindow * /*window*/, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(600, 600, "Animated Triangle", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    glewInit();

    Object obj = initObj();
    glClearColor(0.2f, 0.1f, 0.1f, 1.0f);
    glViewport(0, 0, 600, 600);

    while (!glfwWindowShouldClose(window)) {
        draw(obj);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
