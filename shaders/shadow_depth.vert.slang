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
    float4x4 lightViewProjection;
    float4x4 model;
    uint64_t vertexBuffer;
};

[[vk::push_constant]]
PushConstants pushConstants;

[shader("vertex")]
float4 main(uint vertexIndex : SV_VertexID) : SV_Position
{
    Ptr<Vertex> vertices = reinterpret<Ptr<Vertex>>(pushConstants.vertexBuffer);
    Vertex vertex = vertices[vertexIndex];
    return mul(pushConstants.lightViewProjection, mul(pushConstants.model, float4(vertex.position, 1.0)));
}
