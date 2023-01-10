#version 330 core

in vec2 TexCoords;

uniform sampler2D HDRTexture;
uniform float Exposure;

out vec4 FragColour;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * 0.01 * 1000.0) / (1000.0 + 0.01 - z * (1000.0 - 0.01));
}

void main() {
    const float gamma = 2.2f;
    vec3 HdrColour = texture(HDRTexture, TexCoords).rgb;

    vec3 Mapped = vec3(1.0) - exp(-HdrColour * Exposure); // Tone mapping
    pow(Mapped, vec3(1.0 / gamma)); // Gamma correction

    FragColour = vec4(Mapped, 1.0);
}
