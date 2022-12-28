#version 330 core

struct MaterialProperties {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct LightProperties {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColour;

uniform vec3 ViewPosition; 
uniform MaterialProperties Material;
uniform LightProperties Light;

in vec3 Normal;
in vec3 FragmentPosition;

void main() {
    // Ambient
    vec3 ambient = Material.ambient * Light.ambient;

    // Diffuse
    vec3 LightDirection = normalize(Light.position - FragmentPosition);
    vec3 Norm = normalize(Normal);
    float diff = max(dot(LightDirection, Norm), 0.0);
    vec3 diffuse = (diff * Material.diffuse) * Light.diffuse;

    // Specular
    vec3 viewDir = normalize(ViewPosition - FragmentPosition);
    vec3 reflectDir = reflect(-LightDirection, Norm);  
    float spec = max(dot(viewDir, reflectDir), 0.0);
    vec3 specular = Light.specular * (pow(spec, 1) * Material.specular);  

    vec3 finalColour = (specular + ambient + diffuse);
    FragColour = vec4(finalColour, 1);
}
