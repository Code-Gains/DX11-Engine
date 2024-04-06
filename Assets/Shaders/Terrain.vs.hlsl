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

cbuffer PerFrame : register(b0)
{
    matrix viewprojection;
};

struct PerInstanceData
{
    float4x4 worldMatrix;
    Material material;
};

cbuffer PerInstance : register(b3)
{
    PerInstanceData instanceData[256]; // Max batch size
};

Texture2D HeightMap : register(t0);
SamplerState HeightSampler : register(s0);

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Uv : TEXCOORD0;
};

struct VSOutput
{
    float4 Position : SV_Position;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 PositionWorld : POSITIONWORLD;
    Material Material : MATERIAL;
};

VSOutput Main(VSInput input, uint instanceID : SV_InstanceID)
{
    VSOutput output = (VSOutput)0;

    // Convert the input normal into world space
    float3 normalWorld = normalize(mul(input.Normal, (float3x3)instanceData[instanceID].worldMatrix));

    // Calculate the model-view-projection matrix
    matrix world = mul(viewprojection, instanceData[instanceID].worldMatrix);

    // sample normal map to get correct vertex height
    output.Position = mul(world, float4(input.Position, 1.0));

    output.Uv = input.Uv;

    // The normal does not need to be adjusted if it's purely a displacement along the normal.
    output.Normal = normalWorld;

    // Calculate the world position with the adjusted position
    output.PositionWorld = mul(float4(input.Position, 1.0), instanceData[instanceID].worldMatrix).xyz;

    output.Material = instanceData[instanceID].material;

    return output;
}