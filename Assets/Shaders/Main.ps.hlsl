struct PSInput
{
    float4 Position : SV_Position;
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

float4 RainbowColor(float3 position, float3 camPosition)
{
    float distance = length(position - camPosition); // Calculate the distance

    // Define rainbow colors
    float3 colors[7];
    colors[0] = float3(1.0, 0.0, 0.0); // Red
    colors[1] = float3(1.0, 0.5, 0.0); // Orange
    colors[2] = float3(1.0, 1.0, 0.0); // Yellow
    colors[3] = float3(0.0, 1.0, 0.0); // Green
    colors[4] = float3(0.0, 0.0, 1.0); // Blue
    colors[5] = float3(0.5, 0.0, 1.0); // Indigo
    colors[6] = float3(1.0, 0.0, 1.0); // Violet

    // Map the distance to the rainbow spectrum
    float t = distance / 200; // MaxDistance should be set to the maximum distance you want to consider
    int colorIndex = int(t * 6.0); // 6.0 is the number of color bands
    float3 startColor = colors[colorIndex];
    float3 endColor = colors[colorIndex + 1];
    float3 interpolatedColor = lerp(startColor, endColor, frac(t * 6.0));

    return float4(interpolatedColor, 1.0);
}

float4 Main(PSInput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float3 lightDir = normalize(lightPosition.xyz - input.PositionWorld);

    // Ambient
    float3 ambient = matAmbient.rgb * lightAmbient.rgb;

    // Diffuse
    float3 diff = max(dot(normal, lightDir), 0.0) * matDiffuse.rgb * lightDiffuse.rgb;

    // Specular
    float3 viewDir = normalize(input.PositionWorld - camPosition);
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess) * matSpecular.rgb * lightSpecular.rgb;

    float3 result = ambient + diff + spec;
    //float distance = length(input.PositionWorld - camPosition);
    //float t = saturate((distance / 30.0f)); // Map distance to the range [0, 1]
    //float4 color = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), float4(1.0f, 1.0f, 1.0f, 1.0f), t);
    //return color;
    //return float4(normalize(input.Normal) * 0.5f + 0.5f, 1.0f); // note: normalize to 0-1 range for visualization
    //return RainbowColor(input.PositionWorld - camPosition, camPosition);
    
    return float4(result, 1.0);
}