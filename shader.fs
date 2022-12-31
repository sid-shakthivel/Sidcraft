#version 330 core

uniform sampler2D DiffuseTexture0;

in vec2 TexCoords;
out vec4 FragColour;

void main() {
    FragColour = texture(DiffuseTexture0, TexCoords);
}