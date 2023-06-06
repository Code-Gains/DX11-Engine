struct PSInput
{
    float4 Position : SV_Position;
    float3 Color : COLOR0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 PositionWorld : POSITIONWORLD;
};

struct Material
{
    float4 matAmbient;
    float4 matDiffuse;
    float4 matSpecular;
    float matShininess;
};

struct Light
{
    float4 lightPosition;
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
};

cbuffer Material : register(b0)
{
    float4 matAmbient;
    float4 matDiffuse;
    float4 matSpecular;
    float matShininess;
};

cbuffer Light : register(b1)
{
    float4 lightPosition;
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
};

float4 Main(PSInput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float3 lightDir = normalize(lightPosition.xyz - input.PositionWorld);

    // Ambient
    float3 ambient = matAmbient.rgb * lightAmbient.rgb;

    // Diffuse
    float3 diff = max(dot(normal, lightDir), 0.0) * matDiffuse.rgb * lightDiffuse.rgb;

    // Specular
    float3 viewDir = normalize(-input.PositionWorld);
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess) * matSpecular.rgb * lightSpecular.rgb;

    float3 result = ambient + diff + spec;
    return float4(result, 1.0);
}