[[vk::binding(0, 0)]]
[[vk::image_format("rgba16f")]]
RWTexture2D<float4> image;

[shader("compute")]
[numthreads(16, 16, 1)]
void main(
    uint3 dispatchThreadId : SV_DispatchThreadID,
    uint3 groupThreadId : SV_GroupThreadID)
{
    uint width;
    uint height;
    image.GetDimensions(width, height);

    uint2 texelCoord = dispatchThreadId.xy;
    if (texelCoord.x >= width || texelCoord.y >= height) {
        return;
    }

    float4 color = float4(0.0, 0.0, 0.0, 1.0);

    if (groupThreadId.x != 0 && groupThreadId.y != 0) {
        color.x = float(texelCoord.x) / float(width);
        color.z = float(texelCoord.y) / float(height);
    }

    image[texelCoord] = color;
}
