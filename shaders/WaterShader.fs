#version 330 core

in vec4 ClipSpaceCoords;
in vec2 TexCoords;
in vec3 ToCameraVector;

uniform sampler2D ReflectionTexture;
uniform sampler2D RefractionTexture;
uniform sampler2D MainTexture;
uniform sampler2D DuDvMap;
uniform float MoveFactor; 

layout (location = 0) out vec4 FragColour;

const float WaveStrength = 0.02;

void main() {
    vec2 NDCoords = (ClipSpaceCoords.xy / ClipSpaceCoords.w) / 2.0 + 0.5;

    vec2 ReflectCoords = vec2(NDCoords.x, -NDCoords.y);
    vec2 RefractCoords = vec2(NDCoords.x, NDCoords.y);

    vec2 Distortion1 = (texture(DuDvMap, vec2(TexCoords.x + MoveFactor, TexCoords.y)).rg * 2.0 - 1.0) * WaveStrength;
    vec2 Distortion2 = (texture(DuDvMap, vec2(-TexCoords.x + MoveFactor, TexCoords.y + MoveFactor)).rg * 2.0 - 1.0) * WaveStrength;
    vec2 TotalDistortion = Distortion1;

    ReflectCoords += TotalDistortion;
    ReflectCoords.x = clamp(ReflectCoords.x, 0.001, 0.999);
    ReflectCoords.y = clamp(ReflectCoords.y, -0.999, -0.001);

    RefractCoords += TotalDistortion;
    RefractCoords = clamp(RefractCoords, 0.001, 0.999);

    vec4 ReflectColour = texture(ReflectionTexture, ReflectCoords);
    vec4 RefractColour = texture(RefractionTexture, RefractCoords);
    vec4 WaterColour = texture(MainTexture, TexCoords);

    vec3 ViewVector = normalize(ToCameraVector);
    float RefractiveFactor = dot(ViewVector, vec3(0, 1, 0));
    
    FragColour = mix(mix(ReflectColour, RefractColour, 0.5), WaterColour, 0.25);
}
