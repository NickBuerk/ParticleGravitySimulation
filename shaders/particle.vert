#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 velocity;

layout(location = 0) out vec3 fragColor;

void main() {
  gl_PointSize = 1.0;
  fragColor = vec3(1.0, 1.0, 1.0);
  gl_Position = vec4(position.xy, 0.0, 1.0);
}
