#version 330 core

out vec4 FragColour;

uniform vec3 ObjectColour;
uniform vec3 LightColour;

void main() {
    FragColour = vec4(ObjectColour * LightColour, 1);
}