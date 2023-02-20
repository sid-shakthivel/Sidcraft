#version 330 core

in vec2 TexCoords;

uniform sampler2D Image;

out vec4 FragColour;

float near = 1.0; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {
    // float depthValue = texture(Image, TexCoords).r;
    // FragColour = vec4(vec3(LinearizeDepth(depthValue) / far), 1.0);
    float depthValue = texture(Image, TexCoords).r;
    FragColour = vec4(vec3(depthValue), 1.0);
}