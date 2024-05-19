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

//TextureCube skyboxTexture : register(t0);
Texture2D skyboxTexture : register(t0);
SamplerState skyboxSampler : register(s0);

float4 Main(PSInput input) : SV_Target
{

    float3 direction = normalize(input.PositionWorld);
    float4 sampledColor = skyboxTexture.Sample(skyboxSampler, input.Uv);
    //float4 sampledColor = skyboxTexture.Sample(skyboxSampler, direction);

    // Debug output
    //float4 debugColor = float4(abs(direction), 1.0);
    //return debugColor;

    // Uncomment this line to see the sampled color
    return sampledColor;
    // float3 direction = normalize(input.PositionWorld);
    // //return float4((direction), 1.0);
    // float4 sampledColor = skyboxTexture.Sample(skyboxSampler, direction);
    // return sampledColor;
    // float3 normal = normalize(input.Normal);
    // float3 toLight = lightPosition.xyz - input.PositionWorld;
    // float3 lightDir = normalize(toLight);
    // float distance = length(toLight);

    // float3 diff = max(dot(normal, lightDir), 0.0) * input.Material.matDiffuse.rgb * lightDiffuse.rgb;
    // float3 result = diff;
    // return float4(result, 1.0);
}


    //float distance = length(input.PositionWorld - camPosition);
    //float t = saturate((distance / 30.0f)); // Map distance to the range [0, 1]
    //float4 color = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), float4(1.0f, 1.0f, 1.0f, 1.0f), t);
    //return color;
    //return float4(normalize(input.Normal) * 0.5f + 0.5f, 1.0f); // note: normalize to 0-1 range for visualization
    //return RainbowColor(input.PositionWorld - camPosition, camPosition);

// struct Material
// {
//     float4 matAmbient;
//     float4 matDiffuse;
//     float4 matSpecular;
//     float matShininess;
// };

// cbuffer Camera : register(b1)
// {
//     float3 camPosition;
// };

// cbuffer Light : register(b2)
// {
//     float4 lightPosition;
//     float4 lightAmbient;
//     float4 lightDiffuse;
//     float4 lightSpecular;
// };

// struct PSInput
// {
//     float4 Position : SV_Position;
//     float2 Uv : TEXCOORD0;
//     float3 Normal : NORMAL;
//     float3 PositionWorld : POSITIONWORLD;
//     Material Material : MATERIAL;
// };

// TextureCube skyboxTexture : register(t0);
// SamplerState skyboxSampler : register(s0);

// float4 Main(PSInput input) : SV_Target
// {
//     float4 fallbackColor = float4(1.0f, 0.0f, 1.0f, 1.0f); // Bright red for debug
//     float4 sampledColor = skyboxTexture.Sample(skyboxSampler, input.PositionWorld * 3);

//     // If all components of the sampled color are zero, return the fallback color
//     if (sampledColor.r == 0.0f && sampledColor.g == 0.0f && sampledColor.b == 0.0f)
//     {
//         return fallbackColor;
//     }
//     return sampledColor;

//     //  // Sample the texture at the middle coordinate
//     // float3 middleCoord = float3(0.5f, 0.5f, 0.5f);
//     // float4 sampledColor = skyboxTexture.Sample(skyboxSampler, middleCoord);
    
//     // // Return the sampled color
//     // return sampledColor;
// }

