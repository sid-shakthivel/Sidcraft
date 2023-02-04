#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragmentPosition;
in vec4 FragPosLightSpace;

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

// uniform SpotlightProperties SpotLight;
uniform DirectionalLightProperties DirectionalLight;
uniform sampler2D DiffuseTexture0;
uniform sampler2D ShadowMap;
uniform vec3 ViewPosition;
uniform vec3 LightPosition;

out vec4 FragColour;

float ShadowCalculation(vec4 fragPosLightSpace, float bias) {
    vec3 ProjCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // Perspective division
    ProjCoords = (ProjCoords + 1.0f) / 2.0f ; // Transform to [0,1] range
    float ClosestDepth = texture(ShadowMap, ProjCoords.xy).r; // Get closest depth value from light's perspective
    float CurrentDepth = ProjCoords.z; // Get depth of current fragment from light's perspective
    float shadow = CurrentDepth - bias > ClosestDepth ? 1.0 : 0.0; // Check whether in shadow

    return shadow;
}

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

    float Bias = max(0.05 * (1.0 - dot(Norm, LightDirection)), 0.005);  
    float shadow = ShadowCalculation(FragPosLightSpace, Bias);

    vec3 Directional = ambient + ((diffuse + specular) * (1.0 - shadow));

    // Directional = ambient + (diffuse + specular);

    return Directional;
}

void main() {
    // General Lighting
    vec3 ViewDir = normalize(FragmentPosition - ViewPosition);
    vec3 Directional = CalcDirLight(DirectionalLight, Normal, ViewDir);

    float DepthValue = texture(ShadowMap, TexCoords).r;

    vec3 color = texture(DiffuseTexture0, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(LightPosition - FragmentPosition);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(ViewPosition - FragmentPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float Bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  

    float shadow = ShadowCalculation(FragPosLightSpace, Bias);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color; 

    FragColour = vec4(lighting.x, lighting.y, lighting.z, 1);  

    // Gamma Correction
    // float gamma = 2.2;
    // FragColour.rgb = pow(FragColour.rgb, vec3(1.0/gamma));
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

        // Shadows
        float Bias = max(0.05 * (1.0 - dot(Norm, LightDirection)), 0.005);  
        float shadow = ShadowCalculation(FragPosLightSpace, Bias);

        return vec3(ambient + (diffuse * (1 - shadow)) + (specular * (1 - shadow)));
    } else {
        return ambient;
    }
}

