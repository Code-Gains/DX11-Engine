[[vk::binding(2, 3)]] Sampler2D spotShadowMap;

float SpotShadowVisibility(uint index, float3 position, float3 towardLight) {
    if (sceneData.spotShadowParameters.x != float(index)) return 1.0;
    float4 clip = mul(sceneData.spotLightViewProjection,
        float4(position + towardLight * sceneData.spotShadowParameters.w, 1.0));
    if (clip.w <= sceneData.spotShadowParameters.y) return 1.0;
    float3 projected = clip.xyz / clip.w;
    float2 uv = projected.xy * 0.5 + 0.5;
    if (any(uv < 0.0) || any(uv > 1.0) || projected.z < 0.0 || projected.z > 1.0) return 1.0;
    float storedDepth = spotShadowMap.SampleLevel(uv, 0.0).r;
    return projected.z + 0.000002 >= storedDepth ? 1.0 : 0.0;
}

float3 EvaluateSpotLights(float3 position, float3 normal, float3 view,
    float normalDotView, float alphaRoughness, float3 diffuseColor,
    float3 specularColor, float reflectance90) {
    float3 result = float3(0.0);
    for (uint i = 0; i < min(sceneData.spotLightCounts.x, 8u); ++i) {
        SpotLight light = sceneData.spotLights[i];
        float3 offset = light.positionAndRange.xyz - position;
        float distanceSquared = dot(offset, offset);
        float range = light.positionAndRange.w;
        if (range <= 0.0 || distanceSquared >= range * range || distanceSquared < 1e-8) continue;
        float3 direction = offset * rsqrt(distanceSquared);
        float coneCos = dot(-direction, light.directionAndOuterCos.xyz);
        float cone = saturate((coneCos - light.directionAndOuterCos.w) /
            max(light.coneParameters.x - light.directionAndOuterCos.w, 0.000001));
        cone = cone * cone * (3.0 - 2.0 * cone);
        float normalDotLight = max(dot(normal, direction), 0.0);
        if (normalDotLight <= 0.0 || cone <= 0.0) continue;
        float3 halfVector = direction + view;
        halfVector *= rsqrt(max(dot(halfVector, halfVector), 1e-8));
        float3 fresnel = FresnelSchlick(specularColor, float3(reflectance90), saturate(dot(view, halfVector)));
        float geometry = GeometricOcclusion(normalDotLight, normalDotView, alphaRoughness);
        float distribution = MicrofacetDistribution(saturate(dot(normal, halfVector)), alphaRoughness);
        float3 diffuse = (1.0 - fresnel) * diffuseColor / PI;
        float3 specular = fresnel * geometry * distribution / max(4.0 * normalDotLight * normalDotView, 0.001);
        float ratioSquared = distanceSquared / (range * range);
        float cutoff = saturate(1.0 - ratioSquared * ratioSquared);
        float attenuation = cutoff * cutoff / max(distanceSquared, 1.0);
        float3 radiance = light.colorAndIntensity.rgb * light.colorAndIntensity.w * attenuation * cone;
        result += SpotShadowVisibility(i, position, direction) * normalDotLight * radiance * (diffuse + specular);
    }
    return result;
}
