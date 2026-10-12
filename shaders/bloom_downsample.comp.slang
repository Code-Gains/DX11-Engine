#include "bloom_common.slang"

[shader("compute")]
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (any(id.xy >= bloom.sizes.zw)) return;
    float2 uv = (float2(id.xy) + 0.5) / float2(bloom.sizes.zw);
    float2 step = 0.5 / float2(bloom.sizes.xy);
    float3 color = float3(0.0);
    for (int y = -1; y <= 1; y += 2)
        for (int x = -1; x <= 1; x += 2) {
            float3 sample = ReadBloom(uv + float2(x, y) * step);
            color += bloom.settings.w > 0.5 ? ExtractBloom(sample) : sample;
        }
    destinationImage[id.xy] = float4(min(color * 0.25, float3(60000.0)), 1.0);
}
