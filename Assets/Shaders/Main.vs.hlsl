struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Color : COLOR0;
    float2 Uv : TEXCOORD0;
};

struct VSOutput
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

cbuffer PerFrame : register(b0)
{
    matrix viewprojection;
};

cbuffer PerObject : register(b1)
{
    matrix modelmatrix;
};

VSOutput Main(VSInput input)
{
    VSOutput output = (VSOutput)0;

    matrix world = mul(viewprojection, modelmatrix);
    output.Position = mul(world, float4(input.Position, 1.0));
    output.Color = input.Color;
    output.Uv = input.Uv;
    output.Normal = mul(input.Normal, (float3x3) modelmatrix);
    output.PositionWorld = mul(float4(input.Position, 1.0), modelmatrix).xyz;
    return output;
}