#version 410 core

// layout (location = 0) in vec3 InputPos;
// layout (location = 2) in vec2 InputTexCoords;
// layout (location = 3) in float InputTexIndex;

layout (location = 0) in uint CondensedPos;
layout (location = 1) in uint CondensedOther;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec3 CameraPos;
uniform vec3 LightPos;

out vec4 ClipSpaceCoords;
out vec2 MainTexCoords;
out vec2 AdjustedTexCoords;
out vec3 ToCameraVector;

const float Tiling = 10.0;

float ExtractValue(uint Target, int StartPos, int BitLength) {
    uint Mask = (1u << BitLength) - 1u;
    return float((Target >> StartPos) & Mask);
}

void main() {
    float PosX = ExtractValue(CondensedPos, 0, 6);
    float PosY = ExtractValue(CondensedPos, 6, 6);
    float PosZ = ExtractValue(CondensedPos, 12, 6);

    vec3 InputPos = vec3(PosX, PosY, PosZ);

    float TexCoordX = ExtractValue(CondensedOther, 6, 1);
    float TexCoordY = ExtractValue(CondensedOther, 7, 1);

    float InputTexIndex = ExtractValue(CondensedOther, 8, 8);
    vec2 InputTexCoords = vec2(TexCoordX, TexCoordY);

    float Column = mod(InputTexIndex, 16);
    float Row = floor(InputTexIndex / 16);
    float XOffset = Column / 16;
    float YOffset = Row / 16;

    vec4 WorldPostion = Model * vec4(InputPos, 1.0);    
    ClipSpaceCoords = Projection * View * WorldPostion;
    MainTexCoords = (InputTexCoords / 16) + vec2(XOffset, YOffset);
    AdjustedTexCoords = vec2((InputPos.x / 2.0) + 0.5, (InputPos.z / 2.0) + 0.5) * 0.1;
    ToCameraVector = CameraPos - WorldPostion.xyz;

    gl_Position = ClipSpaceCoords;
}