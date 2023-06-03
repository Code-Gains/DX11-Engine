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
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float Shininess;
};

struct Light
{
    float4 Position;
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
};

cbuffer PerFrame : register(b0)
{
    matrix viewprojection;
};

cbuffer Material : register(b1)
{
    float4 DiffuseColor;
    float SpecularIntensity;
    float SpecularPower;
};

cbuffer Light : register(b2)
{
    float3 LightDirection;
    float3 LightColor;
};

cbuffer PerObject : register(b3)
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
    return output;
}