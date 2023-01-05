#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragmentPosition;

struct DirectionalLightProperties {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct SpotlightProperties {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
};

uniform SpotlightProperties SpotLight;
uniform DirectionalLightProperties DirectionalLight;
uniform sampler2D DiffuseTexture0;
uniform vec3 ViewPosition;

out vec4 FragColour;

vec3 CalcDirLight(DirectionalLightProperties light, vec3 normal, vec3 viewDir) {
    vec3 Norm = normalize(normal);
    vec3 LightDirection = normalize(-light.direction);

    vec3 ambient = vec3(texture(DiffuseTexture0, TexCoords)) * light.ambient;
    float diff = max(dot(LightDirection, Norm), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(DiffuseTexture0, TexCoords));

    vec3 reflectDir = reflect(-LightDirection, Norm);  
    vec3 halfwayVector = normalize(viewDir + LightDirection);

    float spec = max(dot(halfwayVector, normal), 0.0);
    vec3 specular = light.specular * (pow(spec, 32) * vec3(texture(DiffuseTexture0, TexCoords))); 

    vec3 Directional = ambient + diffuse + specular;

    return Directional;
}

vec3 CalcSpotLight(SpotlightProperties Light, vec3 normal, vec3 viewDir, vec3 fragmentPosition) {
    vec3 Norm = normalize(normal);
    vec3 LightDirection = normalize(Light.position - fragmentPosition);
    float theta = dot(LightDirection, normalize(-Light.direction));

    vec3 ambient = vec3(texture(DiffuseTexture0, TexCoords)) * Light.ambient;

    if (theta > Light.cutOff) {
        float diff = max(dot(LightDirection, Norm), 0.0);
        vec3 diffuse = diff * Light.diffuse * vec3(texture(DiffuseTexture0, TexCoords));

        vec3 reflectDir = reflect(-LightDirection, Norm);  

        float spec = max(dot(viewDir, reflectDir), 0.0);
        vec3 specular = Light.specular * (pow(spec, 1) * vec3(texture(DiffuseTexture0, TexCoords))); 

        return vec3(ambient + diffuse + specular);
    } else {
        return ambient;
    }
}

void main() {
    // General Lighting
    vec3 ViewDir = normalize(FragmentPosition - ViewPosition);
    vec3 Directional = CalcDirLight(DirectionalLight, Normal, ViewDir);
    // vec3 Spot = CalcSpotLight(SpotLight, Normal, ViewDir, FragmentPosition);
    vec3 Combined = Directional;

    FragColour = vec4(Combined.x, Combined.y, Combined.z, 1);

    // Gamma Correction
    float gamma = 2.2;
    FragColour.rgb = pow(FragColour.rgb, vec3(1.0/gamma));
}


