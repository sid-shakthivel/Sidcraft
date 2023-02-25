#version 330 core

in vec4 ClipSpaceCoords;
in vec2 MainTexCoords;
in vec2 DistortionTexCoords;
in vec3 ToCameraVector;

uniform sampler2D ReflectionTexture;
uniform sampler2D RefractionTexture;
uniform sampler2D MainTexture;
uniform sampler2D DuDvMap;
uniform sampler2D NormalMap;
uniform float MoveFactor; 
uniform vec3 LightDirection;
layout (location = 0) out vec4 FragColour;

const float WaveStrength = 0.02;
const float ShineDamper = 20.0;
const float Reflectivity = 0.6;

void main() {
    // Lighting stuff
    vec3 LightColour = vec3(1.0);
    vec3 LightDir = normalize(-LightDirection);

    // Convert from clip space to Texture Coordinate space
    vec2 NDCoords = (ClipSpaceCoords.xy / ClipSpaceCoords.w) / 2.0 + 0.5;

    // Use reflection and refraction of the water
    vec2 ReflectCoords = vec2(NDCoords.x, -NDCoords.y);
    vec2 RefractCoords = vec2(NDCoords.x, NDCoords.y);

    // Apply suitable distortion according to DuDV map
    // vec2 Distortion1 = (texture(DuDvMap, vec2(DistortionTexCoords.x + MoveFactor, DistortionTexCoords.y)).xy * 2.0 - 1.0) * WaveStrength;
    // vec2 Distortion2 = (texture(DuDvMap, vec2(-DistortionTexCoords.x, DistortionTexCoords.y - MoveFactor)).xy * 2.0 - 1.0) * WaveStrength;
    // vec2 TotalDistortion = Distortion1 + Distortion2;

    vec2 TestTexCoords = texture(DuDvMap, vec2(DistortionTexCoords.x + MoveFactor, DistortionTexCoords.y)).rg * 0.1;
	TestTexCoords = TestTexCoords + vec2(DistortionTexCoords.x, DistortionTexCoords.y + MoveFactor);
	vec2 TotalDistortion = (texture(DuDvMap, TestTexCoords).rg * 2.0 - 1.0) * WaveStrength;

    ReflectCoords += TotalDistortion;
    ReflectCoords.x = clamp(ReflectCoords.x, 0.001, 0.999);
    ReflectCoords.y = clamp(ReflectCoords.y, -0.999, -0.001);

    RefractCoords += TotalDistortion;
    RefractCoords = clamp(RefractCoords, 0.001, 0.999);

    // Extract colours from textures
    vec4 ReflectColour = texture(ReflectionTexture, ReflectCoords);
    vec4 RefractColour = texture(RefractionTexture, RefractCoords);
    vec4 WaterColour = texture(MainTexture, MainTexCoords);

    // Fresnel effect
    vec3 ViewVector = normalize(ToCameraVector);
    float RefractiveFactor = dot(ViewVector, vec3(0, 1, 0));

    // Handle normals
    vec4 NormalMapColour = texture(NormalMap, TestTexCoords);
    vec3 Normal = normalize(vec3(NormalMapColour.r * 2.0 - 1.0, NormalMapColour.b, NormalMapColour.g * 2.0 - 1.0));

    vec3 ReflectedLight = reflect(normalize(LightDir), Normal);
    float Specular = pow(max(dot(ReflectedLight, normalize(ToCameraVector)), 0.0), ShineDamper);
    vec3 SpecularHighlights = LightColour * Specular * Reflectivity;

    // Put it all together
    FragColour = mix(mix(ReflectColour, RefractColour, 0.5), WaterColour, 0.2) + vec4(SpecularHighlights, 0.0);

    // FragColour = texture(DuDvMap, DistortionTexCoords);
}
