#version 330 core

in vec4 ClipSpaceCoords;

uniform sampler2D ReflectionTexture;
uniform sampler2D RefractionTexture;

layout (location = 0) out vec4 FragColour;

void main() {
    vec2 NDCoords = (ClipSpaceCoords.xy / ClipSpaceCoords.w) / 2.0 + 0.5;

    vec4 ReflectColour = texture(ReflectionTexture, vec2(NDCoords.x, -NDCoords.y));
    vec4 RefractColour = texture(RefractionTexture, vec2(NDCoords.x, NDCoords.y));

    FragColour = mix(ReflectColour, RefractColour, 0.5);
}

