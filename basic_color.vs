#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertexColor;

uniform mat4 m, v, p;
out vec3 fragmentColor;

void main() {
    gl_Position = p * v * m * vec4(aPos, 1.0);
    fragmentColor = vertexColor;
}
