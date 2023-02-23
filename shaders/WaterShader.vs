#version 330 core

layout (location = 0) in vec3 InputPos;
layout (location = 1) in vec3 InputNormal;
layout (location = 2) in vec2 InputTexCoords;
layout (location = 3) in float InputTexIndex;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec3 CameraPos;

out vec4 ClipSpaceCoords;
out vec2 MainTexCoords;
out vec2 DistortionTexCoords;
out vec3 ToCameraVector;

void main() {
    float Column = mod(InputTexIndex, 16);
    float Row = floor(InputTexIndex / 16);
    float XOffset = Column / 16;
    float YOffset = Row / 16;

    vec4 WorldPostion = Model * vec4(InputPos, 1.0);
    ClipSpaceCoords = Projection * View * WorldPostion;
    MainTexCoords = (InputTexCoords / 16) + vec2(XOffset, YOffset);
    DistortionTexCoords = InputTexCoords;
    ToCameraVector = CameraPos - WorldPostion.xyz;

    gl_Position = ClipSpaceCoords;
}