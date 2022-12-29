#version 330 core

in vec3 Normal;
in vec3 FragmentPosition;
in vec2 TexCoords;

uniform vec3 ViewPosition;

out vec4 FragColour;

struct MaterialProperties {
    sampler2D diffuse;
    sampler2D specular;
};
uniform MaterialProperties Material;

// struct DirectionalLightProperties {
//     vec3 direction;
  
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };  
// uniform DirectionalLightProperties DirectionalLight;

// struct PointLightProperties {
//     vec3 position;
  
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;

//     float constant;
//     float linear;
//     float quadratic;  
// };  
// uniform PointLightProperties PointLight;

struct SpotlightProperties {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
};
uniform SpotlightProperties SpotLight;

// Each light consists of ambient, diffuse and specular
vec3 CalculateSpotlight() 
{
    vec3 LightDirection = normalize(SpotLight.position - FragmentPosition);
    vec3 Norm = normalize(Normal);
    float theta = dot(LightDirection, normalize(-SpotLight.direction));

    vec3 ambient = vec3(texture(Material.diffuse, TexCoords)) * SpotLight.ambient;

    if (theta > SpotLight.cutOff) {
        float diff = max(dot(LightDirection, Norm), 0.0);
        vec3 diffuse = diff * SpotLight.diffuse * vec3(texture(Material.diffuse, TexCoords));

        vec3 viewDir = normalize(FragmentPosition - ViewPosition);
        vec3 reflectDir = reflect(-LightDirection, Norm);  

        float spec = max(dot(viewDir, reflectDir), 0.0);
        vec3 specular = SpotLight.specular * (pow(spec, 1) * vec3(texture(Material.specular, TexCoords))); 

        return vec3(ambient + diffuse + specular);
    } else {
        return ambient;
    }
}

void main() {
    vec3 outputVec = vec3(0);

    // output += CalculateDirectionalLight();

    // output += CalculatePointLight();

    // outputVec += ;

    FragColour = vec4(CalculateSpotlight(), 1);
}

// vec3 CalculateDirectionalLight() 
// {
//     vec3 Norm = normalize(Normal);
//     vec3 LightDirection = normalize(-DirectionalLight.position);

//     vec3 ambient = vec3(texture(Material.diffuse, TexCoords)) * DirectionalLight.ambient;

//     float diff = max(dot(LightDirection, Norm), 0.0);
//     vec3 diffuse = diff * DirectionalLight.diffuse * vec3(texture(Material.diffuse, TexCoords));

//     vec3 viewDir = normalize(FragmentPosition - ViewPosition);
//     vec3 reflectDir = reflect(-LightDirection, Norm);  
//     float spec = max(dot(viewDir, reflectDir), 0.0);
//     vec3 specular = DirectionalLight.specular * (pow(spec, 1) * vec3(texture(Material.specular, TexCoords))); 

//     return diffuse + ambient + specular;
// }

// vec3 CalculatePointLight() 
// {
//     vec3 Norm = normalize(Normal);
//     float dist = length(PointLight.position - FragmentPosition);
//     float attenuation = 1.0 / (Light.constant + Light.linear * dist + Light.quadratic * (dist * dist));

//     vec3 LightDirection = normalize(PointLight.position - FragmentPosition);

//     vec3 ambient = vec3(texture(Material.diffuse, TexCoords)) * DirectionalLight.ambient;

//     float diff = max(dot(LightDirection, Norm), 0.0);
//     vec3 diffuse = diff * DirectionalLight.diffuse * vec3(texture(Material.diffuse, TexCoords));

//     vec3 viewDir = normalize(FragmentPosition - ViewPosition);
//     vec3 reflectDir = reflect(-LightDirection, Norm);  
//     float spec = max(dot(viewDir, reflectDir), 0.0);
//     vec3 specular = DirectionalLight.specular * (pow(spec, 1) * vec3(texture(Material.specular, TexCoords))); 

//     return (diffuse + ambient + specular) * attenuation;
// }

 

