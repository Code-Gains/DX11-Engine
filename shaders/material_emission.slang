float3 DecodeEmissionSrgb(float3 color)
{
    float3 low = color / 12.92;
    float3 high = pow((color + 0.055) / 1.055, float3(2.4));
    return float3(color.x <= 0.04045 ? low.x : high.x,
        color.y <= 0.04045 ? low.y : high.y,
        color.z <= 0.04045 ? low.z : high.z);
}

float3 EvaluateEmission(float3 sampledEmission, float4 materialEmission, float4 emissionOverride)
{
    // File-backed emission images are stored as UNORM; glTF defines their RGB as sRGB.
    float3 textureEmission = DecodeEmissionSrgb(sampledEmission);
    float3 mask = emissionOverride.w > 0.5 ? textureEmission : float3(1.0);
    return textureEmission * materialEmission.rgb + mask * emissionOverride.rgb;
}
