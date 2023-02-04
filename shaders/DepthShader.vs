#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
    mat4 test = lightSpaceMatrix * model;
    gl_Position = lightSpaceMatrix * vec4(pos, 1);
}