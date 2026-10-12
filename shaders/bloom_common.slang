[[vk::binding(0, 0)]] Sampler2D sourceImage;
[[vk::binding(1, 0)]]
[[vk::image_format("rgba16f")]] RWTexture2D<float4> destinationImage;

struct BloomPushConstants {
    uint4 sizes; // source width/height, destination width/height
    float4 settings; // threshold, knee, intensity, mode
};
[[vk::push_constant]] BloomPushConstants bloom;

float3 ReadBloom(float2 uv) {
    uint width, height;
    sourceImage.GetDimensions(width, height);
    float2 validSize = float2(bloom.sizes.xy);
    uv = clamp(uv, 0.5 / validSize, 1.0 - 0.5 / validSize);
    return max(sourceImage.SampleLevel(uv * validSize / float2(width, height), 0.0).rgb, float3(0.0));
}

float3 ExtractBloom(float3 color) {
    float brightness = max(color.r, max(color.g, color.b));
    float knee = bloom.settings.x * bloom.settings.y;
    float soft = clamp(brightness - bloom.settings.x + knee, 0.0, 2.0 * knee);
    soft = soft * soft / max(4.0 * knee, 0.00001);
    return color * (max(brightness - bloom.settings.x, soft) / max(brightness, 0.00001));
}
