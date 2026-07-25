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
    float4 color;
    float4 params;
    float4 corruptionColor;
    float4 corruptionParams;
    float4 cameraPosition;
    uint64_t vertexBuffer;
};

[[vk::push_constant]]
PushConstants pushConstants;

struct VertexOutput
{
    float4 position : SV_Position;
    float3 worldPosition : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float4 color : COLOR0;
    float4 params : TEXCOORD2;
    float3 cameraPosition : TEXCOORD3;
    float4 corruptionColor : COLOR1;
    float4 corruptionParams : TEXCOORD4;
    float effectAge : TEXCOORD5;
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
    float3x3 normalMatrix = InverseTranspose3x3((float3x3)pushConstants.model);

    VertexOutput output;
    output.position = mul(pushConstants.viewProjection, worldPosition);
    output.worldPosition = worldPosition.xyz;
    output.normal = normalize(mul(normalMatrix, vertex.normal));
    output.color = pushConstants.color;
    output.params = pushConstants.params;
    output.cameraPosition = pushConstants.cameraPosition.xyz;
    output.corruptionColor = pushConstants.corruptionColor;
    output.corruptionParams = pushConstants.corruptionParams;
    output.effectAge = pushConstants.cameraPosition.w;
    return output;
}
