#include "bloom_common.slang"

[shader("compute")]
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (any(id.xy >= bloom.sizes.zw)) return;
    float2 uv = (float2(id.xy) + 0.5) / float2(bloom.sizes.zw);
    float4 base = destinationImage[id.xy];
    if (bloom.settings.w > 0.5) {
        destinationImage[id.xy] = float4(min(base.rgb + ReadBloom(uv) * bloom.settings.z,
            float3(60000.0)), base.a);
        return;
    }
    float2 step = 1.0 / float2(bloom.sizes.xy);
    float3 blurred = float3(0.0);
    for (int y = -1; y <= 1; ++y)
        for (int x = -1; x <= 1; ++x) {
            float weight = (x == 0 ? 2.0 : 1.0) * (y == 0 ? 2.0 : 1.0);
            blurred += ReadBloom(uv + float2(x, y) * step) * weight;
        }
    blurred /= 16.0;
    // Normalize the pyramid blend so its brightness does not depend on level count.
    float3 color = lerp(base.rgb, blurred, 0.5);
    destinationImage[id.xy] = float4(min(color, float3(60000.0)), base.a);
}
