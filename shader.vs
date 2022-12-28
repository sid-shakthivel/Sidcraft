#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragmentPosition;
out vec2 TexCoords;

void main() {
    Normal = normal; 
    FragmentPosition = vec3(model * vec4(pos, 1.0));
    TexCoords = texCoords;
    gl_Position = projection * view * model * vec4(pos.x, pos.y, pos.z, 1);
}