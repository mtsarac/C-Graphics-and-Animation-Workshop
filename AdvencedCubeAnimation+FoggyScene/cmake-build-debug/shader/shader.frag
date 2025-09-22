#version 330 core
in vec3 ourColor;
in float distanceToCamera;

out vec4 FragColor;

uniform vec3 fogColor;
uniform float fogNear;
uniform float fogFar;

void main() {
  float fogFactor = clamp((fogFar - distanceToCamera) / (fogFar - fogNear), 0.0, 1.0);
  vec3 color = mix(fogColor, ourColor, fogFactor);
  FragColor = vec4(color, 1.0);
}