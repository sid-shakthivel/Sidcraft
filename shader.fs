#version 330 core

in vec2 TexCoords;
uniform sampler2D DiffuseTexture0;

out vec4 FragColour;

void main() {
    FragColour = texture(DiffuseTexture0, TexCoords);
    // FragColour = vec4(1, 0, 0, 1);
}