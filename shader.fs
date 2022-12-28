#version 330 core

struct MaterialProperties {
    sampler2D diffuse;
    sampler2D specular;
};

struct LightProperties {
    vec3 position;
    // vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColour;

uniform vec3 ViewPosition; 
uniform MaterialProperties Material;
uniform LightProperties Light;

in vec3 Normal;
in vec3 FragmentPosition;
in vec2 TexCoords;

void main() {
    // Ambient
    vec3 ambient = vec3(texture(Material.diffuse, TexCoords)) * Light.ambient;

    // Diffuse
    vec3 LightDirection = normalize(Light.position - FragmentPosition);
    // vec3 LightDirection = normalize(-Light.direction);
    vec3 Norm = normalize(Normal);
    float diff = max(dot(LightDirection, Norm), 0.0);
    vec3 diffuse = diff * Light.diffuse * vec3(texture(Material.diffuse, TexCoords));

    // // Specular
    vec3 viewDir = normalize(FragmentPosition - ViewPosition);
    vec3 reflectDir = reflect(-LightDirection, Norm);  
    float spec = max(dot(viewDir, reflectDir), 0.0);
    vec3 specular = Light.specular * (pow(spec, 1) * vec3(texture(Material.specular, TexCoords)));  

    vec3 finalColour = (specular + ambient + diffuse);
    FragColour = vec4(finalColour, 1);
}
