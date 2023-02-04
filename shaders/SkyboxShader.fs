#version 330 core

uniform samplerCube Cubemap;
uniform samplerCube Cubemap2;
uniform float BlendFactor;
uniform vec3 FogColour;

in vec3 TexCoords;

out vec4 FragColour;

const float LowerLimit = 0.0;
const float UpperLimit = 30.0;

void main()
{             
    vec4 texture1 = texture(Cubemap, TexCoords);
    vec4 texture2 = texture(Cubemap2, TexCoords);

    vec4 FinalColour = mix(texture1, texture2, BlendFactor);

    float Factor = (TexCoords.y - LowerLimit) / (UpperLimit - LowerLimit);
    Factor = clamp(Factor, 0.0, 1.0);

    FragColour = mix(vec4(FogColour, 1.0), FinalColour, Factor);
}  