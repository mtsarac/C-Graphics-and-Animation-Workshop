#include <Windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // perspective, translate
#include <glm/gtc/type_ptr.hpp>        // value_ptr

#include "wavefront.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>



float aspect_ratio=1;

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
        GLfloat z;
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
    };

    std::vector<Vertex> vertices = {
        // Ön yüzey
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}, // Kırmızı
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}, // Kırmızı
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}, // Kırmızı
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}, // Kırmızı
        // Arka yüzey
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // Yeşil
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // Yeşil
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // Yeşil
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // Yeşil
        // Sol yüzey
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}, // Mavi
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}, // Mavi
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}, // Mavi
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}, // Mavi
        // Sağ yüzey
        {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}}, // Sarı
        {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}}, // Sarı
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}}, // Sarı
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}}, // Sarı
        // Üst yüzey
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}}, // Camgöbeği
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}}, // Camgöbeği
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}}, // Camgöbeği
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}}, // Camgöbeği
        // Alt yüzey
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}}, // Mor
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}}, // Mor
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}}, // Mor
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}} // Mor
    };

    std::vector<GLuint> indices = {
        // Ön yüzey
        0, 1, 2, 2, 3, 0,
        // Arka yüzey
        4, 5, 6, 6, 7, 4,
        // Sol yüzey
        8, 9, 10, 10, 11, 8,
        // Sağ yüzey
        12, 13, 14, 14, 15, 12,
        // Üst yüzey
        16, 17, 18, 18, 19, 16,
        // Alt yüzey
        20, 21, 22, 22, 23, 20
    };

    obj.count = vertices.size();

    // Create vertex buffer object
    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // Create vertex array object
    glGenVertexArrays(1, &(obj.vao));
    glBindVertexArray(obj.vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Element buffer'ı (EBO) bağlayın ve verileri yükleyin
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Setup vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // Unbind VAO to prevent accidental changes
    glBindVertexArray(0);

    return obj;
}

void draw(Object obj, glm::mat4 view, glm::mat4 projection) {
    static float angle = 30.0f;
    angle += 0.1f; // Dönme hızını ayarlamak için

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(obj.program);

    GLuint modelLoc = glGetUniformLocation(obj.program, "model");
    GLuint viewLoc = glGetUniformLocation(obj.program, "view");
    GLuint projectionLoc = glGetUniformLocation(obj.program, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(obj.vao);

    // Izgara şeklinde küpler çiz
    for (int x = -5; x <= 5; ++x) {
        for (int y = -10; y <= 10; ++y) {
            for (int z = -5; z <= 5; ++z) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x * 2.0f, y * 2.0f, z * 2.0f));
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
            }


        }
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    // Aspect ratio'yu hesapla
    aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(600, 600, "Foggy Scene", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    Object obj = initObj();

    // Framebuffer size callback fonksiyonunu ayarla
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback
    );

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Fog color (gray)
    glViewport(0, 0, 600, 600);
    glEnable(GL_DEPTH_TEST);

    // Fog parameters
    GLuint fogColorLoc = glGetUniformLocation(obj.program, "fogColor");
    GLuint fogNearLoc = glGetUniformLocation(obj.program, "fogNear");
    GLuint fogFarLoc = glGetUniformLocation(obj.program, "fogFar");

    glUseProgram(obj.program);
    glUniform3f(fogColorLoc, 0.4f, 0.5f, 0.5f); // Fog color (gray)
    glUniform1f(fogNearLoc, 5.0f); // Near distance
    glUniform1f(fogFarLoc, 30.0f); // Far distance

    while (!glfwWindowShouldClose(window)) {
        //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -7.0f, -20.0f));
        //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, -20.0f));
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, -25.0f),glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(30.0f), aspect_ratio, 0.1f, 100.0f);

        draw(obj, view, projection);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
