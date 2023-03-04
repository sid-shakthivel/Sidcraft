#version 410 core

in vec2 TexCoords;

uniform sampler2D Scene;
uniform sampler2D BloomBlur;
uniform float Exposure;

out vec4 FragColour;

const float gamma = 2.2;

void main() {
    vec3 CombinedColour = texture(Scene, TexCoords).rgb + texture(BloomBlur, TexCoords).rgb;

    float test = Exposure;

    // vec3 Mapped = vec3(1.0) - exp(-CombinedColour * Exposure); // Tone mapping
    // vec3 Final = pow(CombinedColour, vec3(1.0 / gamma)); // Gamma correction

    FragColour = vec4(CombinedColour, 1.0);
}
