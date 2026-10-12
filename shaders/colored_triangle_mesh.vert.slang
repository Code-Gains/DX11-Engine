struct Vertex
{
    float3 position;
    float uv_x;
    float3 normal;
    float uv_y;
    float4 tangent;
};

struct PushConstants
{
    float4x4 viewProjection;
    float4x4 model;
    float4 baseColorFactor;
    float4 flashColorAndAmount;
    float4 corruptionColorAndAmount;
    float4 corruptionParams;
    float4 materialEmission;
    float4 emissionOverride;
    uint64_t vertexBuffer;
};

[[vk::push_constant]]
PushConstants pushConstants;

struct VertexOutput
{
    float4 position : SV_Position;
    [[vk::location(0)]] float3 worldPosition : TEXCOORD0;
    [[vk::location(1)]] float3 normal : TEXCOORD1;
    [[vk::location(2)]] float2 uv : TEXCOORD2;
    [[vk::location(3)]] float4 tangent : TEXCOORD3;
    [[vk::location(4)]] float4 baseColorFactor : COLOR0;
    [[vk::location(5)]] float4 flashColorAndAmount : COLOR1;
    [[vk::location(6)]] float4 corruptionColorAndAmount : COLOR2;
    [[vk::location(7)]] float4 corruptionParams : TEXCOORD4;
    [[vk::location(12)]] float4 materialEmission : TEXCOORD7;
    [[vk::location(13)]] float4 emissionOverride : TEXCOORD8;
};

float3x3 InverseTranspose3x3(float3x3 matrix)
{
    float3 row0 = matrix[0];
    float3 row1 = matrix[1];
    float3 row2 = matrix[2];

    float3 cof0 = cross(row1, row2);
    float3 cof1 = cross(row2, row0);
    float3 cof2 = cross(row0, row1);

    float invDet = 1.0 / dot(row0, cof0);
    return float3x3(cof0 * invDet, cof1 * invDet, cof2 * invDet);
}

[shader("vertex")]
VertexOutput main(uint vertexIndex : SV_VertexID)
{
    Ptr<Vertex> vertices = reinterpret<Ptr<Vertex>>(pushConstants.vertexBuffer);
    Vertex vertex = vertices[vertexIndex];

    float4 worldPosition = mul(pushConstants.model, float4(vertex.position, 1.0));
    float3x3 model3x3 = (float3x3)pushConstants.model;
    float3x3 normalMatrix = InverseTranspose3x3(model3x3);

    VertexOutput output;
    output.position = mul(pushConstants.viewProjection, worldPosition);
    output.worldPosition = worldPosition.xyz;
    output.normal = normalize(mul(normalMatrix, vertex.normal));
    output.uv = float2(vertex.uv_x, vertex.uv_y);
    output.tangent = float4(normalize(mul(model3x3, vertex.tangent.xyz)), vertex.tangent.w);
    output.baseColorFactor = pushConstants.baseColorFactor;
    output.flashColorAndAmount = pushConstants.flashColorAndAmount;
    output.corruptionColorAndAmount = pushConstants.corruptionColorAndAmount;
    output.corruptionParams = pushConstants.corruptionParams;
    output.materialEmission = pushConstants.materialEmission;
    output.emissionOverride = pushConstants.emissionOverride;
    return output;
}
