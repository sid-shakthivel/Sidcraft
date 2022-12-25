#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 colour;
layout (location = 2) in vec2 textureCoord;
out vec4 vertexColour;
out vec2 textureCoordinate;

void main() {
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
    vertexColour = vec4(colour.x, colour.y, colour.z, 1.0);
    textureCoordinate = textureCoord;
}