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

cbuffer DirectionalLight : register(b2)
{
    float4 direction;
    float4 ambient;
    float4 diffuse;
    float4 specular;
};

struct PSInput
{
    float4 Position : SV_Position;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 PositionWorld : POSITIONWORLD;
    Material Material : MATERIAL;
};

SamplerState NormalSampler : register(s0);
Texture2D NormalMap : register(t1);

float4 Main(PSInput input) : SV_Target
{
    // Sample the normal map and remap from [0, 1] to [-1, 1]
    float3 normalFromMap = NormalMap.Sample(NormalSampler, input.Uv).xyz * 2.0 - 1.0;
    //float3 normal = normalize(normalFromMap);
    float3 normal = normalFromMap;

    float3 lightDir = normalize(-direction.xyz);

    // Ambient
    float3 ambientColor = input.Material.matAmbient.rgb * ambient.rgb;

    // Diffuse
    float3 diff = max(dot(normal, lightDir), 0.0) * input.Material.matDiffuse.rgb * diffuse.rgb;

    // Specular
    float3 viewDir = normalize(camPosition - input.PositionWorld);
    float3 reflectDir = reflect(-lightDir, normal);
    float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), input.Material.matShininess);
    float3 specularColor = specStrength * input.Material.matSpecular.rgb * specular.rgb;

    // Fresnel Effect
    float fresnel = pow(1.0 - max(dot(viewDir, normal), 0.0), 3.0) * 0.5 + 0.5;
    specularColor *= fresnel;

    // Combine lighting components
    float3 result = ambientColor + diff + specularColor;
    return float4(result, 1.0);
}