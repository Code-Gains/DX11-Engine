struct LineVertex
{
    float3 position;
    float pad0;
    float4 color;
};

struct PushConstants
{
    float4x4 viewProjection;
    uint64_t vertexBuffer;
};

[[vk::push_constant]]
PushConstants pushConstants;

struct VertexOutput
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};

struct FragmentInput
{
    float4 color : COLOR0;
};

[shader("vertex")]
VertexOutput vertexMain(uint vertexIndex : SV_VertexID)
{
    Ptr<LineVertex> vertices = reinterpret<Ptr<LineVertex>>(pushConstants.vertexBuffer);
    LineVertex vertex = vertices[vertexIndex];

    VertexOutput output;
    output.position = mul(pushConstants.viewProjection, float4(vertex.position, 1.0));
    output.color = vertex.color;
    return output;
}

[shader("fragment")]
float4 fragmentMain(FragmentInput input) : SV_Target
{
    return input.color;
}
