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

out vec4 FragColour;

float ShadowCalculation(vec4 fragPosLightSpace, float bias) {
    vec3 ProjCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // Perspective division
    ProjCoords = ProjCoords * 0.5 + 0.5; // Transform to [0,1] range
    // float ClosestDepth = texture(ShadowMap, ProjCoords.xy).r; // Get closest depth value from light's perspective
    float CurrentDepth = ProjCoords.z; // Get depth of current fragment from light's perspective
    // float shadow = CurrentDepth - bias > ClosestDepth ? 1.0 : 0.0; // Check whether in shadow
    
    float shadow = 0.0;
    vec2 TexelSize = 1.0 / textureSize(ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float PCFDepth = texture(ShadowMap, ProjCoords.xy + vec2(x, y) * TexelSize).r; 
            shadow += CurrentDepth - bias > PCFDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

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

    vec3 Directional = ambient + ((diffuse + specular) * (1 - shadow));

    return Directional;
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

