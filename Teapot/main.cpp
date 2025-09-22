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
        MessageBox(0, infoLog, "Msg title", MB_OK);
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
        MessageBox(0, infoLog, "Msg title", MB_OK);
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
        MessageBox(0, infoLog, "Msg title", MB_OK);
    }
    glValidateProgram(obj.program);
    glGetProgramiv(obj.program, GL_VALIDATE_STATUS, &status);
    if (!status) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(obj.program, 1024, nullptr, infoLog);
        std::cout << "Error validating program: " << infoLog << std::endl;
    }

    // Load 3D model data from a file
    std::vector<wavefront::Vertex> vertices;
    wavefront::load("03_Teapot/teapot.obj", vertices);
    //wavefront::load("04_Earth/earth.obj", vertices);
    //wavefront::load("03_Teapot/teapot.obj", vertices);
    obj.count = vertices.size();

    // Create vertex buffer object
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Create vertex array object
    glGenVertexArrays(1, &(obj.vao));
    glBindVertexArray(obj.vao);

    // Bind and upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(wavefront::Vertex), vertices.data(), GL_STATIC_DRAW);

    // Setup vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(wavefront::Vertex), (void*)offsetof(wavefront::Vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(wavefront::Vertex), (void *)(offsetof(wavefront::Vertex, tex)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(wavefront::Vertex), (void*)offsetof(wavefront::Vertex, nor));
    glEnableVertexAttribArray(2);

    // Unbind VAO to prevent accidental changes
    glBindVertexArray(0);

    return obj;
}



void draw(Object obj, glm::mat4 view, glm::mat4 projection) {
    static float angle = 0.0f;
    angle += 0.2f; // Dönme hızını ayarlamak için

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(obj.program);

    GLuint modelLoc = glGetUniformLocation(obj.program, "model");
    GLuint viewLoc = glGetUniformLocation(obj.program, "view");
    GLuint projectionLoc = glGetUniformLocation(obj.program, "projection");
    GLuint normalMatrixLoc = glGetUniformLocation(obj.program, "normalMatrix");
    GLuint lightPosLoc = glGetUniformLocation(obj.program, "lightPos");
    GLuint viewPosLoc = glGetUniformLocation(obj.program, "viewPos");
    GLuint lightColorLoc = glGetUniformLocation(obj.program, "lightColor");
    GLuint objectColorLoc = glGetUniformLocation(obj.program, "objectColor");

    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(view * model));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(glm::vec3(0.0f, 7.0f, 10.0f)));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
    glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.7f, 0.3f)));

    glBindVertexArray(obj.vao);
    glDrawArrays(GL_TRIANGLES, 0, obj.count); // Use glDrawArrays for simplicity

    glBindVertexArray(0);
}



void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
}


int main() {
    // GLFW ve pencere oluşturma işlemleri
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(600, 600, "Teapot", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    // Load the external model
    Object obj = initObj();
    glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    framebuffer_size_callback(window, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);



    while (!glfwWindowShouldClose(window)) {
        glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);
        draw(obj, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
