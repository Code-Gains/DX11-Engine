struct FragmentInput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

[[vk::binding(0, 0)]]
Texture2DMS<float> depthTexture;

[shader("fragment")]
float4 main(FragmentInput input) : SV_Target
{
    int2 pixel = int2(input.position.xy);
    float depth = depthTexture.Load(pixel, 0);
    float visibleDepth = pow(saturate(depth), 0.35);
    return float4(visibleDepth.xxx, 1.0);
}
