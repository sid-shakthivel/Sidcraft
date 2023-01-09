#version 330 core

in vec2 TexCoords;

uniform sampler2D HDRTexture;
uniform float Exposure;

out vec4 FragColour;

void main() {
    const float gamma = 2.2f;
    vec3 HdrColour = texture(HDRTexture, TexCoords).rgb;

    vec3 Mapped = vec3(1.0) - exp(-HdrColour * Exposure); // Tone mapping
    pow(Mapped, vec3(1.0 / gamma)); // Gamma correction

    FragColour = vec4(Mapped, 1.0);
}
