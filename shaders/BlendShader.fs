#version 330 core

in vec2 TexCoords;

uniform sampler2D Scene;
uniform sampler2D BloomBlur;
uniform float Exposure;

out vec4 FragColour;

void main() {
    const float gamma = 2.2f;
    vec3 CombinedColour = texture(Scene, TexCoords).rgb + texture(BloomBlur, TexCoords).rgb;

    vec3 Mapped = vec3(1.0) - exp(-CombinedColour * Exposure); // Tone mapping
    vec3 Final = pow(Mapped, vec3(1.0 / gamma)); // Gamma correction

    FragColour = vec4(Final, 1.0);
}
