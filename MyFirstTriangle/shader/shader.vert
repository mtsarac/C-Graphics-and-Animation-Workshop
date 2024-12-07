#version 430 core

layout (location = 0) in vec2 aPosition;

uniform vec2 randomOffset;

void main() {
  gl_Position = vec4(aPosition+randomOffset, 0.0, 1.0);
}
