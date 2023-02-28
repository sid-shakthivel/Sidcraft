#version 410 core
layout (location = 0) in vec3 InputPos;

uniform mat4 Model;
// uniform mat4 LightSpaceMatrix;

void main() {
    // gl_Position = LightSpaceMatrix * Model * vec4(InputPos, 1.0);
    gl_Position = Model * vec4(InputPos, 1.0);
}