struct PushConstants
{
    float4x4 viewProjection;
    float4x4 model;
    float4 params;
    uint64_t vertexBuffer;
};

[[vk::push_constant]]
PushConstants pushConstants;

[shader("fragment")]
float main() : SV_Target
{
    return 1.0;
}
