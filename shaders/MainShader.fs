#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    float Visibility;
    float InputTexIndex;
} FSInput;

uniform sampler2D MainTexture;
uniform sampler2D ShadowMap;

uniform vec3 LightDirection; 
uniform vec3 ViewPos;
uniform vec3 SkyColour;

layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColour;

const float Gamma = 2.2;

float CalculateShadow(vec4 FragPosLightSpace, float Bias)
{
    // Perform perspective divide
    vec3 ProjCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;

    // Transform to [0,1] range (NDC)
    ProjCoords = ProjCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float ClosestDepth = texture(ShadowMap, ProjCoords.xy).r; 

    // Get depth of current fragment from light's perspective
    float CurrentDepth = ProjCoords.z;

    // Perform Percentage Closer Filtering (PCF) which produces softer shadows
    float Shadow = 0.0;
    vec2 TexelSize = 1.0 / textureSize(ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float PCFDepth = texture(ShadowMap, ProjCoords.xy + vec2(x, y) * TexelSize).r; 
            Shadow += CurrentDepth - Bias > PCFDepth ? 1.0 : 0.0;        
        }    
    }
    Shadow /= 9.0;

    return Shadow;
}

void main()
{           
    // Setup basic variables
    vec3 Colour = texture(MainTexture, FSInput.TexCoords).rgb;
    vec3 Normal = normalize(FSInput.Normal);
    vec3 LightColour = vec3(1.0);

    // If the InputTexIndex is 0, it must be a light     
    if (FSInput.InputTexIndex == 35.0) {
        Colour = vec3(15.0, 15.0, 15.0);
    }

    // Perform Blinn-Phong lighting mode starting with ambient light
    vec3 Ambient = 0.15 * LightColour;

    // Diffuse lighting
    vec3 LightDir = normalize(-LightDirection);
    float Diff = max(dot(LightDir, Normal), 0.0);
    vec3 Diffuse = Diff * LightColour;

    // Specular lighting
    vec3 ViewDir = normalize(ViewPos - FSInput.FragPos);
    vec3 ReflectDir = reflect(-LightDir, Normal);
    vec3 HalfwayDir = normalize(LightDir + ViewDir);  
    float Spec = pow(max(dot(Normal, HalfwayDir), 0.0), 64.0);
    vec3 Specular = Spec * LightColour;    

    // Calculate the shadow using a bias
    float Bias = max(0.05 * (1.0 - dot(Normal, LightDir)), 0.005);  
    float Shadow = CalculateShadow(FSInput.FragPosLightSpace, Bias);          

    // Put everything together
    vec3 Lighting = (Ambient + (1.0 - Shadow) * (Diffuse + Specular)) * mix(SkyColour, Colour, FSInput.Visibility);    

    FragColour = vec4(Lighting, 1.0);
    if (FragColour.rgb == vec3(0,0,0)) discard;

    // Perform gamma correction
    FragColour.rgb = pow(FragColour.rgb, vec3(1.0/Gamma));

    // Bloom stuff  
    float Brightness = dot(FragColour.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(Brightness > 1.0) {
        BrightColour = vec4(FragColour.rgb, 1.0);
    }
    else {
        BrightColour = vec4(0.0, 0.0, 0.0, 1.0);
    }
}

