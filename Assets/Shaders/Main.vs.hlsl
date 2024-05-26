struct Material
{
    float4 matAmbient;
    float4 matDiffuse;
    float4 matSpecular;
    float matShininess;
};

struct DirectionalLight
{
    float4 direction;
    float4 ambient;
    float4 diffuse;
    float4 specular;
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
    VSOutput output = (VSOutput) 0;

    matrix world = mul(viewprojection, instanceData[instanceID].worldMatrix);
    output.Position = mul(world, float4(input.Position, 1.0));
    
    output.Uv = input.Uv;
    
    output.Normal = mul(input.Normal, (float3x3)instanceData[instanceID].worldMatrix);
    
    output.PositionWorld = mul(float4(input.Position, 1.0), instanceData[instanceID].worldMatrix).xyz;
    
    output.Material = instanceData[instanceID].material;
    
    return output;
}