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

struct PSInput
{
    float4 Position : SV_Position;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 PositionWorld : POSITIONWORLD;
    Material Material : MATERIAL;
};

TextureCube skyboxTexture : register(t0);
SamplerState skyboxSampler : register(s0);

float4 Main(PSInput input) : SV_Target
{

    float3 direction = normalize(input.PositionWorld);
    float4 sampledColor = skyboxTexture.Sample(skyboxSampler, direction);
    return sampledColor;
}

