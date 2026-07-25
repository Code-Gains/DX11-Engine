struct PushConstants
{
    float4 data1;
    float4 data2;
    float4 data3;
    float4 data4;
};

[[vk::binding(0, 0)]]
[[vk::image_format("rgba8")]]
RWTexture2D<float4> image;

[[vk::push_constant]]
PushConstants pushConstants;

float Noise2d(float2 value)
{
    float xhash = cos(value.x * 37.0);
    float yhash = cos(value.y * 57.0);
    return frac(415.92653 * (xhash + yhash));
}

float NoisyStarField(float2 samplePosition, float threshold)
{
    float starValue = Noise2d(samplePosition);
    if (starValue >= threshold) {
        starValue = pow((starValue - threshold) / (1.0 - threshold), 6.0);
    } else {
        starValue = 0.0;
    }

    return starValue;
}

float StableStarField(float2 samplePosition, float threshold)
{
    float fractX = frac(samplePosition.x);
    float fractY = frac(samplePosition.y);
    float2 floorSample = floor(samplePosition);

    float v1 = NoisyStarField(floorSample, threshold);
    float v2 = NoisyStarField(floorSample + float2(0.0, 1.0), threshold);
    float v3 = NoisyStarField(floorSample + float2(1.0, 0.0), threshold);
    float v4 = NoisyStarField(floorSample + float2(1.0, 1.0), threshold);

    return
        v1 * (1.0 - fractX) * (1.0 - fractY) +
        v2 * (1.0 - fractX) * fractY +
        v3 * fractX * (1.0 - fractY) +
        v4 * fractX * fractY;
}

float4 MainImage(float2 fragCoord, float2 resolution)
{
    float3 color = pushConstants.data1.xyz * fragCoord.y / resolution.y;

    float starFieldThreshold = pushConstants.data1.w;
    float xRate = 0.2;
    float yRate = -0.06;
    float2 samplePosition = fragCoord + float2(xRate * float(1), yRate * float(1));
    float starValue = StableStarField(samplePosition, starFieldThreshold);
    color += float3(starValue);

    return float4(color, 1.0);
}

[shader("compute")]
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint width;
    uint height;
    image.GetDimensions(width, height);

    uint2 texelCoord = dispatchThreadId.xy;
    if (texelCoord.x >= width || texelCoord.y >= height) {
        return;
    }

    float4 color = MainImage(float2(texelCoord), float2(width, height));
    image[texelCoord] = color;
}
