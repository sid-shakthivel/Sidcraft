#version 330 core

layout (location = 0) in vec3 InputPos;
layout (location = 1) in vec3 InputNormal;
layout (location = 2) in vec2 InputTexCoords;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

out vec4 ClipSpaceCoords;

void main() {
    ClipSpaceCoords = Projection * View * Model * vec4(InputPos, 1.0);
    gl_Position = ClipSpaceCoords;
}