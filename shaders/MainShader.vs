#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aTextureIndex;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    float Visibility;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

const float density = 0.007;
const float gradient = 0.5;

void main()
{
    float Column = mod(aTextureIndex, 16);
    float XOffset = Column / 16;
    float Row = floor(aTextureIndex / 16);
    float YOffset = Row / 16;

    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = (aTexCoords / 16) + vec2(XOffset, YOffset);
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    vec4 PosRelativeCam = view * model * vec4(aPos, 1.0);
    float Distance = length(PosRelativeCam.xyz);
    vs_out.Visibility = exp(-pow(Distance * density, gradient));
    // vs_out.Visibility = clamp(vs_out.Visibility, 0.0, 1.0);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}