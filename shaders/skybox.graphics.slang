struct PushConstants
{
    float4x4 viewProjection;
};

[[vk::push_constant]]
PushConstants pushConstants;

static const float3 positions[36] = {
    float3(-1, -1, -1), float3(-1,  1, -1), float3( 1,  1, -1),
    float3( 1,  1, -1), float3( 1, -1, -1), float3(-1, -1, -1),

    float3(-1, -1,  1), float3( 1,  1,  1), float3(-1,  1,  1),
    float3( 1,  1,  1), float3(-1, -1,  1), float3( 1, -1,  1),

    float3(-1,  1,  1), float3( 1,  1,  1), float3( 1,  1, -1),
    float3( 1,  1, -1), float3(-1,  1, -1), float3(-1,  1,  1),

    float3(-1, -1,  1), float3(-1, -1, -1), float3( 1, -1, -1),
    float3( 1, -1, -1), float3( 1, -1,  1), float3(-1, -1,  1),

    float3( 1, -1,  1), float3( 1, -1, -1), float3( 1,  1, -1),
    float3( 1,  1, -1), float3( 1,  1,  1), float3( 1, -1,  1),

    float3(-1, -1,  1), float3(-1,  1, -1), float3(-1, -1, -1),
    float3(-1,  1, -1), float3(-1, -1,  1), float3(-1,  1,  1)
};

struct VertexOutput
{
    float4 position : SV_Position;
    float3 direction : TEXCOORD0;
};

struct FragmentInput
{
    float3 direction : TEXCOORD0;
};

[[vk::binding(0, 0)]]
SamplerCube skyboxTexture;

[shader("vertex")]
VertexOutput vertexMain(uint vertexIndex : SV_VertexID)
{
    float3 position = positions[vertexIndex];
    float4 clipPosition = mul(pushConstants.viewProjection, float4(position, 1.0));

    VertexOutput output;
    output.position = clipPosition.xyww;
    output.direction = position;
    return output;
}

[shader("fragment")]
float4 fragmentMain(FragmentInput input) : SV_Target
{
    float3 color = skyboxTexture.Sample(normalize(input.direction)).rgb;
    return float4(color, 1.0);
}
