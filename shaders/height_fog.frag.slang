struct FragmentInput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

struct PushConstants
{
    float4x4 inverseViewProjection;
    float4 cameraPosition;
    float4 fogCenterAndRadius;
    float4 fogColorAndDensity;
    float4 fogParams;
};

[[vk::binding(0, 0)]]
Texture2D<float> depthTexture;

[[vk::push_constant]]
PushConstants pushConstants;

float3 ReconstructWorldPosition(float2 uv, float depth)
{
    float2 ndc = uv * 2.0 - 1.0;
    float4 clipPosition = float4(ndc, depth, 1.0);
    float4 worldPosition = mul(pushConstants.inverseViewProjection, clipPosition);
    return worldPosition.xyz / worldPosition.w;
}

float ComputeFogOpacity(float3 rayOrigin, float3 rayDirection, float viewDistance)
{
    float3 fogCenter = pushConstants.fogCenterAndRadius.xyz;
    float planetRadius = pushConstants.fogCenterAndRadius.w;
    float fogHeight = max(0.001, pushConstants.fogParams.x);
    float density = max(0.0, pushConstants.fogColorAndDensity.w);
    float distanceLimit = fogHeight * max(0.001, pushConstants.fogParams.y);
    float maxOpacity = saturate(pushConstants.fogParams.z);
    float clampedDistance = min(max(0.0, viewDistance), distanceLimit);

    const int sampleCount = 8;
    float stepLength = clampedDistance / float(sampleCount);
    float accumulatedDensity = 0.0;

    [unroll]
    for (int i = 0; i < sampleCount; ++i) {
        float sampleDistance = (float(i) + 0.5) * stepLength;
        float3 samplePosition = rayOrigin + rayDirection * sampleDistance;
        float altitude = max(0.0, length(samplePosition - fogCenter) - planetRadius);
        accumulatedDensity += exp(-altitude / fogHeight) * stepLength;
    }

    float densityFactor = 1.0 - exp(-accumulatedDensity * density);
    return maxOpacity * saturate(densityFactor);
}

[shader("fragment")]
float4 main(FragmentInput input) : SV_Target
{
    if (input.uv.x < 0.0 || input.uv.x > 1.0 ||
        input.uv.y < 0.0 || input.uv.y > 1.0) {
        discard;
    }

    int2 pixel = int2(input.position.xy);
    float depth = depthTexture.Load(int3(pixel, 0));
    if (pushConstants.fogParams.w < 0.0) {
        return float4(pushConstants.fogColorAndDensity.rgb, -pushConstants.fogParams.w);
    }

    if (depth <= 0.000001) {
        float3 cameraPosition = pushConstants.cameraPosition.xyz;
        float3 farWorldPosition = ReconstructWorldPosition(input.uv, 0.0);
        float3 rayToFar = farWorldPosition - cameraPosition;
        float viewDistance = length(rayToFar);
        float skyFogOpacity = ComputeFogOpacity(
            cameraPosition,
            rayToFar / max(viewDistance, 0.000001),
            viewDistance);
        if (skyFogOpacity <= 0.0) {
            discard;
        }
        return float4(pushConstants.fogColorAndDensity.rgb, skyFogOpacity);
    }

    float3 worldPosition = ReconstructWorldPosition(input.uv, depth);
    float3 cameraPosition = pushConstants.cameraPosition.xyz;
    float3 rayToWorld = worldPosition - cameraPosition;
    float viewDistance = length(rayToWorld);
    float opacity = ComputeFogOpacity(
        cameraPosition,
        rayToWorld / max(viewDistance, 0.000001),
        viewDistance);

    return float4(pushConstants.fogColorAndDensity.rgb, opacity);
}
