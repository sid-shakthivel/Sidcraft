#version 330 core
layout (location = 0) in vec3 pos;
// layout (location = 1) in vec3 colour;
layout (location = 1) in vec2 textureCoord;
out vec2 textureCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(pos.x, pos.y, pos.z, 1);
    textureCoordinate = vec2(textureCoord.x, textureCoord.y);
}