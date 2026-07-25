struct FragmentInput
{
    float3 worldPosition : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float4 color : COLOR0;
    float4 params : TEXCOORD2;
    float3 cameraPosition : TEXCOORD3;
    float4 corruptionColor : COLOR1;
    float4 corruptionParams : TEXCOORD4;
    float effectAge : TEXCOORD5;
};

float Hash13(float3 value)
{
    value = frac(value * 0.1031);
    value += dot(value, value.yzx + 33.33);
    return frac((value.x + value.y) * value.z);
}

[shader("fragment")]
float4 main(FragmentInput input) : SV_Target
{
    float fresnelPower = max(input.params.x, 0.01);
    float fresnelIntensity = max(input.params.y, 0.0);
    float baseIntensity = max(input.params.z, 0.0);
    float alphaMultiplier = clamp(input.params.w, 0.0, 1.0);

    float3 normal = normalize(input.normal);
    float3 viewDirection = normalize(input.cameraPosition - input.worldPosition);
    float normalDotView = clamp(abs(dot(normal, viewDirection)), 0.0, 1.0);
    float fresnel = pow(1.0 - normalDotView, fresnelPower);

    float corruptionScale = max(input.corruptionParams.x, 0.001);
    float corruptionSoftness = clamp(input.corruptionParams.y, 0.001, 1.0);
    float corruptionIntensity = max(input.corruptionParams.z, 0.0);
    float corruptionAmount = clamp(input.corruptionParams.w, 0.0, 1.0);

    float baseStep = floor(input.effectAge * 17.0);
    float holdNoise = Hash13(float3(baseStep * 0.37, 21.0, 5.0));
    float holdLength = lerp(1.0, 4.0, step(0.55, holdNoise));
    float glitchStep = floor(baseStep / holdLength);
    float modeNoise = Hash13(float3(glitchStep, 31.0, 2.0));
    float coverageNoise = Hash13(float3(glitchStep, 41.0, 9.0));
    float blinkNoise = Hash13(float3(glitchStep, 53.0, 4.0));
    float smallCutPhase = step(0.46, modeNoise);
    float blinkPhase = step(0.73, blinkNoise);

    float scaleMode = lerp(
        lerp(0.14, 0.62, Hash13(float3(glitchStep, 61.0, 1.0))),
        lerp(2.2, 5.6, Hash13(float3(glitchStep, 67.0, 3.0))),
        smallCutPhase);
    float3 glitchOffset = float3(
        Hash13(float3(glitchStep, 1.0, 7.0)),
        Hash13(float3(glitchStep, 2.0, 8.0)),
        Hash13(float3(glitchStep, 3.0, 9.0))) * lerp(9.0, 41.0, smallCutPhase);
    float3 cellPosition = input.worldPosition * corruptionScale * scaleMode + glitchOffset;
    float3 cell = floor(cellPosition);
    float cellNoise = Hash13(cell + floor(normal * 7.0));
    float detailNoise = Hash13(floor(cellPosition * 2.0) + normal * 19.0);
    float stripeNoise = step(
        lerp(0.78, 0.55, smallCutPhase),
        frac(dot(floor(cellPosition), float3(0.17, 0.31, 0.43)) + glitchStep * 0.37));
    float corruptionNoise = lerp(cellNoise, detailNoise, 0.35);
    corruptionNoise = lerp(corruptionNoise, stripeNoise, lerp(0.12, 0.34, smallCutPhase));
    float amountPulse = lerp(0.48, 1.82, coverageNoise);
    amountPulse *= lerp(1.15, 0.62, smallCutPhase);
    amountPulse = lerp(amountPulse, lerp(0.0, 0.22, Hash13(float3(glitchStep, 71.0, 6.0))), blinkPhase);
    float threshold = 1.0 - clamp(corruptionAmount * amountPulse, 0.0, 1.0);
    float corruptionMask = smoothstep(
        threshold - corruptionSoftness,
        threshold + corruptionSoftness,
        corruptionNoise);

    float3 faceColor = input.color.rgb * (baseIntensity + fresnel * fresnelIntensity);
    float3 corruptionColor = input.corruptionColor.rgb * (baseIntensity + fresnel * fresnelIntensity + corruptionIntensity);
    float3 color = lerp(faceColor, corruptionColor, corruptionMask * input.corruptionColor.a);
    float alpha = input.color.a * alphaMultiplier * clamp(0.45 + fresnel * 0.75, 0.0, 1.0);
    alpha = max(alpha, input.corruptionColor.a * corruptionMask * alphaMultiplier);

    return float4(color, alpha);
}
