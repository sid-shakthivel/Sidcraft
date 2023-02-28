#version 410 core

in vec2 TexCoords;

// uniform sampler2D Image;
uniform sampler2DArray Image;
uniform int Layer;

float TestLayer = 3;

out vec4 FragColour;

float near = 0.01; 
float far  = 1000.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {
    // Depth Stuff
    // float depthValue = texture(Image, TexCoords).r;
    // FragColour = vec4(vec3(LinearizeDepth(depthValue) / far), 1.0);
    // FragColour = vec4(vec3(depthValue), 1.0);

    float test = Layer;

    // CSM stuff
    float DepthValue = texture(Image, vec3(TexCoords, TestLayer)).r;
    FragColour = vec4(vec3(DepthValue), 1.0);

    // Normal stuff
    // FragColour = texture(Image, TexCoords);
}