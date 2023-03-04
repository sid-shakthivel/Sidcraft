#version 410 core
layout (location = 0) in vec3 InputPos;
layout (location = 1) in vec3 InputNormal;
layout (location = 2) in vec2 InputTexCoords;
layout (location = 3) in float InputTexIndex;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform mat4 LightSpaceMatrix;
uniform float RunningTime;
uniform vec4 HorizontalPlane;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    float Visibility;
    float InputTexIndex;
} VSOutput;

const float Density = 0.02; // Thickness of fog
const float Gradient = 500; // Determines how quickly visibility decreases with distance

void main()
{
    // Handle texture atlas
    float Column = mod(InputTexIndex, 16);
    float Row = floor(InputTexIndex / 16);
    float XOffset = Column / 16;
    float YOffset = Row / 16;

    // Compute output variables for FS
    VSOutput.FragPos = vec3(Model * vec4(InputPos, 1.0));
    VSOutput.Normal = transpose(inverse(mat3(Model))) * InputNormal;
    VSOutput.TexCoords = (InputTexCoords / 16) + vec2(XOffset, YOffset);
    VSOutput.InputTexIndex = InputTexIndex;

    // Handle clip distance for reflections
    gl_ClipDistance[0] = dot(vec4(VSOutput.FragPos, 1.0), HorizontalPlane);

    // Handle fog
    vec4 PosRelativeCam = View * Model * vec4(InputPos, 1.0);
    float Distance = length(PosRelativeCam.xyz);
    VSOutput.Visibility = exp(-pow((Distance * Density), Gradient));
    VSOutput.Visibility = clamp(VSOutput.Visibility, 0.0, 1.0);
    VSOutput.Visibility = 1;

    // If time is greater then 1, use the time value to modify positions of trees
    if (RunningTime > 0.0) {
        // Transform the position by offset using sin
        float WaveX = InputPos.x + 0.1 * sin(3 * (RunningTime + 5.0 * InputPos.z));
        float WaveY = InputPos.y + 0.1 * sin(3 * (RunningTime + 5.0 * InputPos.x));
        float WaveZ = InputPos.z + 0.1 * sin(3 * (RunningTime + 5.0 * InputPos.x));

        gl_Position = Projection * View * Model * vec4(WaveX, WaveY, WaveZ ,1.0);
    } else {
        gl_Position = Projection * View * Model * vec4(InputPos, 1.0);
    }
}