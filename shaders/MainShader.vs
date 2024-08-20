#version 410 core

precision highp uint;

layout (location = 0) in uint CondensedPos;
layout (location = 1) in uint CondensedOther;

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

float ExtractValue(uint Target, int StartPos, int BitLength) {
    uint Mask = (1u << BitLength) - 1u;
    return float((Target >> StartPos) & Mask);
}

void main()
{
    float PosX = ExtractValue(CondensedPos, 0, 6);
    float PosY = ExtractValue(CondensedPos, 6, 6);
    float PosZ = ExtractValue(CondensedPos, 12, 6);
    vec3 InputPos = vec3(PosX, PosY, PosZ);

    float NormalX = ExtractValue(CondensedOther, 0, 2) - 2;
    float NormalY = ExtractValue(CondensedOther, 2, 2) - 2;
    float NormalZ = ExtractValue(CondensedOther, 4, 2) - 2;

    float TexCoordX = ExtractValue(CondensedOther, 6, 1);
    float TexCoordY = ExtractValue(CondensedOther, 7, 1);

    float InputTexIndex = ExtractValue(CondensedOther, 8, 8);

    vec3 InputNormal = vec3(NormalX, NormalY, NormalZ);
    vec2 InputTexCoords = vec2(TexCoordX, TexCoordY);

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
    // if (RunningTime > 0.0) {
    //     // Transform the position by offset using sin
    //     float WaveX = InputPos.x + 0.1 * sin(3 * (RunningTime + 5.0 * InputPos.z));
    //     float WaveY = InputPos.y + 0.1 * sin(3 * (RunningTime + 5.0 * InputPos.x));
    //     float WaveZ = InputPos.z + 0.1 * sin(3 * (RunningTime + 5.0 * InputPos.x));

    //     gl_Position = Projection * View * Model * vec4(WaveX, WaveY, WaveZ ,1.0);
    // } else {
    //     gl_Position = Projection * View * Model * vec4(InputPos, 1.0);
    // }

    float WaveX = InputPos.x + 0.1 * sin(3 * (RunningTime + 5.0 * InputPos.z));

    gl_Position = Projection * View * Model * vec4(InputPos, 1.0);
}