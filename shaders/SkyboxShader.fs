#version 330 core

uniform samplerCube Cubemap;
uniform samplerCube Cubemap2;
uniform float BlendFactor;

in vec3 TexCoords;

out vec4 FragColor;

void main()
{             
    vec4 texture1 = texture(Cubemap, TexCoords);
    vec4 texture2 = texture(Cubemap2, TexCoords);

    FragColor = mix(texture1, texture2, BlendFactor);
}  