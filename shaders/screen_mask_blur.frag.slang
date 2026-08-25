struct FragmentInput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

struct PushConstants
{
    float4 texelSizeAndRadius;
    float4 directionAndPad;
};

[[vk::push_constant]]
PushConstants pushConstants;

[[vk::binding(0, 0)]]
Sampler2D sourceMaskTexture;

float SampleMask(float2 uv)
{
    return sourceMaskTexture.Sample(saturate(uv)).r;
}

[shader("fragment")]
float main(FragmentInput input) : SV_Target
{
    float radius = max(pushConstants.texelSizeAndRadius.z, 0.0);
    float2 radiusStep =
        pushConstants.texelSizeAndRadius.xy *
        pushConstants.directionAndPad.xy *
        radius;

    if (radius <= 0.001) {
        return SampleMask(input.uv);
    }

    float mask = 0.0;
    float weightSum = 0.0;
    static const int sampleRadius = 16;

    for (int i = -sampleRadius; i <= sampleRadius; ++i) {
        float normalizedOffset = float(i) / float(sampleRadius);
        float weight = exp(-normalizedOffset * normalizedOffset * 4.0);
        mask += SampleMask(input.uv + radiusStep * normalizedOffset) * weight;
        weightSum += weight;
    }

    return saturate(mask / max(weightSum, 0.0001) * 2.0);
}
