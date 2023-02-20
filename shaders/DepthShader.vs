#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 LightSpaceMatrix;

void main() {
    mat4 test = LightSpaceMatrix * model;
    gl_Position = LightSpaceMatrix * model * vec4(pos, 1.0);
}