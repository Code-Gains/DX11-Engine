struct Material
{
    float4 matAmbient;
    float4 matDiffuse;
    float4 matSpecular;
    float matShininess;
};

cbuffer Camera : register(b1)
{
    float3 camPosition;
};

cbuffer Light : register(b2)
{
    float4 lightPosition;
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
};

cbuffer Material : register(b3)
{
    float4 matAmbient;
    float4 matDiffuse;
    float4 matSpecular;
    float matShininess;
};

struct PSInput
{
    float4 Position : SV_Position;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 PositionWorld : POSITIONWORLD;
    Material Material : MATERIAL;
};

float4 Main(PSInput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float3 toLight = lightPosition.xyz - input.PositionWorld;
    float3 lightDir = normalize(toLight);
    float distance = length(toLight);
    // Adjust the attenuation formula or remove it to ensure the diffuse term is more visible
    float attenuation = 1.0 / (1.0 + 0.001 * distance + 0.001 * distance * distance);

    // Ambient
    float3 ambient = input.Material.matAmbient.rgb * lightAmbient.rgb;

    // Diffuse
    float3 diff = max(dot(normal, lightDir), 0.0) * input.Material.matDiffuse.rgb * lightDiffuse.rgb;

    // Specular
    float3 viewDir = normalize(camPosition - input.PositionWorld);
    float3 reflectDir = reflect(-lightDir, normal);
    float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), input.Material.matShininess);
    float3 specular = specStrength * input.Material.matSpecular.rgb * lightSpecular.rgb;

    // Fresnel Effect
    float fresnel = pow(1.0 - max(dot(viewDir, normal), 0.0), 3.0) * 0.5 + 0.5;
    specular += fresnel * specular;

    // Combine lighting components with adjusted attenuation and fresnel effect for specular highlight
    float3 result = ambient + (diff + specular) * attenuation;

    return float4(result, 1.0);
}


    //float distance = length(input.PositionWorld - camPosition);
    //float t = saturate((distance / 30.0f)); // Map distance to the range [0, 1]
    //float4 color = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), float4(1.0f, 1.0f, 1.0f, 1.0f), t);
    //return color;
    //return float4(normalize(input.Normal) * 0.5f + 0.5f, 1.0f); // note: normalize to 0-1 range for visualization
    //return RainbowColor(input.PositionWorld - camPosition, camPosition);