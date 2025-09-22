#version 330 core

layout(location = 0) in vec2 aPos;

uniform vec2 offset;  // Offset for random position change over time

void main() {
    gl_Position = vec4(aPos + offset, 0.0f, 1.0f);
}
