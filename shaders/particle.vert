#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 velocity;
layout(location = 2) in vec4 color;

layout(location = 0) out vec4 fragColor;

out gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
};

void main() {
  gl_PointSize = 0.1;
  fragColor = color;
  gl_Position = vec4(position.xy, 0.0, 1.0);
}
