#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;

void main() {
    TexCoords = aPos;
    mat4 best  = projection * model;
    vec4 Pos = projection * model * vec4(aPos, 1.0);
    gl_Position = Pos.xyww;
}