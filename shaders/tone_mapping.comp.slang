[[vk::binding(0, 0)]]
[[vk::image_format("rgba16f")]]
RWTexture2D<float4> sceneImage;

struct ToneMappingPushConstants
{
    float exposure;
};

[[vk::push_constant]]
ToneMappingPushConstants pushConstants;

float3 ACESFilm(float3 color)
{
    return saturate((color * (2.51 * color + 0.03)) /
        (color * (2.43 * color + 0.59) + 0.14));
}

float3 LinearToSrgb(float3 color)
{
    return float3(
        color.r <= 0.0031308 ? color.r * 12.92 : 1.055 * pow(color.r, 1.0 / 2.4) - 0.055,
        color.g <= 0.0031308 ? color.g * 12.92 : 1.055 * pow(color.g, 1.0 / 2.4) - 0.055,
        color.b <= 0.0031308 ? color.b * 12.92 : 1.055 * pow(color.b, 1.0 / 2.4) - 0.055);
}

[shader("compute")]
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint width, height;
    sceneImage.GetDimensions(width, height);
    if (dispatchThreadId.x >= width || dispatchThreadId.y >= height) return;
    uint2 pixel = dispatchThreadId.xy;
    float4 hdr = sceneImage[pixel];
    // Tone mapping and display conversion happen once, after scene composition.
    sceneImage[pixel] = float4(LinearToSrgb(ACESFilm(max(hdr.rgb, float3(0.0)) * pushConstants.exposure)), hdr.a);
}
