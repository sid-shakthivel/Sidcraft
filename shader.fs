#version 330 core

in vec4 vertexColour;
in vec2 textureCoordinate;

out vec4 FragColour;
uniform sampler2D chessTexture;

void main() {
    FragColour = texture(chessTexture, textureCoordinate);
}