#version 410 core

layout (location = 0) in vec3 InputPos;
layout (location = 1) in vec2 InputTexCoords;

uniform mat4 Model;

out vec2 TexCoords;

void main() {
    TexCoords = InputTexCoords;
    gl_Position =  Model * vec4(InputPos, 1.0);
}