#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 TextureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float NumberOfRows;
uniform float TestIndex;

out vec3 Normal;
out vec3 FragmentPosition;
out vec2 TexCoords;

void main() {
    int BestIndex = int(TestIndex);

    float Column = mod(BestIndex, NumberOfRows);
    float XOffset = Column / NumberOfRows;
    float Row = floor(BestIndex / NumberOfRows);
    float YOffset = Row / NumberOfRows;

    FragmentPosition = vec3(model * vec4(pos, 1.0));
    Normal = normal;
    TexCoords = (TextureCoordinates / NumberOfRows) + vec2(XOffset, YOffset);
    gl_Position = projection * view * model * vec4(pos.x, pos.y, pos.z, 1);
}