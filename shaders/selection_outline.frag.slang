struct FragmentInput
{
    float2 uv : TEXCOORD0;
};

struct PushConstants
{
    float4 color;
    float2 texelSize;
    float thickness;
    float pad0;
};

[[vk::binding(0, 0)]]
Sampler2D selectionMask;

[[vk::push_constant]]
PushConstants pushConstants;

[shader("fragment")]
float4 main(FragmentInput input) : SV_Target
{
    if (input.uv.x < 0.0 || input.uv.x > 1.0 ||
        input.uv.y < 0.0 || input.uv.y > 1.0) {
        discard;
    }

    float center = selectionMask.Sample(input.uv).r;
    if (center > 0.5) {
        discard;
    }

    bool nearSelection = false;
    int radius = int(ceil(pushConstants.thickness));

    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            float2 offset = float2(float(x), float(y));
            if (length(offset) > pushConstants.thickness + 0.5) {
                continue;
            }

            float2 sampleUv = input.uv + offset * pushConstants.texelSize;
            if (sampleUv.x < 0.0 || sampleUv.x > 1.0 ||
                sampleUv.y < 0.0 || sampleUv.y > 1.0) {
                continue;
            }

            float sampleMask = selectionMask.Sample(sampleUv).r;
            if (sampleMask > 0.5) {
                nearSelection = true;
            }
        }
    }

    if (!nearSelection) {
        discard;
    }

    return pushConstants.color;
}
