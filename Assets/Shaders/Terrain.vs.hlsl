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

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Uv : TEXCOORD0;
    float Height : TEXCOORD1;
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

    // Calculate the model-view-projection matrix
    matrix world = mul(viewprojection, instanceData[instanceID].worldMatrix);
    //matrix world = mul(viewprojection, modelMatrix);
    output.Position = mul(world, float4(input.Position, 1.0));
    
    output.Uv = input.Uv;
    
    // Transform the normal
    output.Normal = mul(input.Normal, (float3x3)instanceData[instanceID].worldMatrix);
    //output.Normal = mul(input.Normal, (float3x3)modelMatrix);
    
    // Calculate the world position
    output.PositionWorld = mul(float4(input.Position, 1.0), instanceData[instanceID].worldMatrix).xyz;
    //output.PositionWorld = mul(float4(input.Position, 1.0), modelMatrix).xyz;
    
    output.Material = instanceData[instanceID].material;
    
    return output;
}