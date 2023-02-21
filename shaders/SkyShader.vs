#version 330 core
layout (location = 0) in vec3 InputPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 TexCoords;

void main() {
    TexCoords = InputPos;
    gl_Position = Projection * Model * vec4(InputPos, 1.0);
}