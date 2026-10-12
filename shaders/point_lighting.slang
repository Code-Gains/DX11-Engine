// Included after SceneData and the material's existing PBR helpers.
[[vk::binding(1, 3)]]
SamplerCube pointShadowMap;

float PointShadowVisibility(uint lightIndex, float3 offsetFromLight)
{
    if (sceneData.pointShadowParameters.x != float(lightIndex)) return 1.0;
    float distance = length(offsetFromLight);
    float nearPlane = sceneData.pointShadowParameters.y;
    float farPlane = sceneData.pointShadowParameters.z;
    float bias = sceneData.pointShadowParameters.w;
    float3 biasedOffset = offsetFromLight * (max(distance - bias, nearPlane) / max(distance, 1e-8));
    float majorDistance = max(max(abs(biasedOffset.x), abs(biasedOffset.y)), abs(biasedOffset.z));
    if (majorDistance <= nearPlane) return 1.0;
    // Same reversed perspective depth as the six shadow views (not radial distance).
    float receiverDepth = nearPlane * (farPlane - majorDistance) / (majorDistance * (farPlane - nearPlane));
    float storedDepth = pointShadowMap.SampleLevel(offsetFromLight, 0.0).r;
    return receiverDepth + 0.000002 >= storedDepth ? 1.0 : 0.0;
}

float3 EvaluatePointLights(float3 position, float3 normal, float3 view,
    float normalDotView, float alphaRoughness, float3 diffuseColor,
    float3 specularColor, float reflectance90)
{
    float3 result = float3(0.0);
    for (uint i = 0; i < min(sceneData.pointLightCounts.x, 16u); ++i) {
        float3 offset = sceneData.pointLights[i].positionAndRange.xyz - position;
        float range = sceneData.pointLights[i].positionAndRange.w;
        float distanceSquared = dot(offset, offset);
        if (range <= 0.0 || distanceSquared >= range * range || distanceSquared < 1e-8) continue;
        float3 direction = offset * rsqrt(distanceSquared);
        float normalDotLight = max(dot(normal, direction), 0.0);
        if (normalDotLight <= 0.0) continue;
        float3 halfVector = direction + view;
        halfVector *= rsqrt(max(dot(halfVector, halfVector), 1e-8));
        float3 fresnel = FresnelSchlick(specularColor, float3(reflectance90), saturate(dot(view, halfVector)));
        float geometry = GeometricOcclusion(normalDotLight, normalDotView, alphaRoughness);
        float distribution = MicrofacetDistribution(saturate(dot(normal, halfVector)), alphaRoughness);
        float3 diffuse = (1.0 - fresnel) * diffuseColor / PI;
        float3 specular = fresnel * geometry * distribution / max(4.0 * normalDotLight * normalDotView, 0.001);
        // Inverse-square falloff, softened within one unit and smoothly cut off at range.
        float ratioSquared = distanceSquared / (range * range);
        float cutoff = saturate(1.0 - ratioSquared * ratioSquared);
        float attenuation = cutoff * cutoff / max(distanceSquared, 1.0);
        float3 radiance = sceneData.pointLights[i].colorAndIntensity.rgb *
            sceneData.pointLights[i].colorAndIntensity.w * attenuation;
        float visibility = PointShadowVisibility(i, -offset);
        result += visibility * normalDotLight * radiance * (diffuse + specular);
    }
    return result;
}
