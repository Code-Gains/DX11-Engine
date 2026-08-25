struct FragmentInput
{
    float3 worldPosition : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float2 uv : TEXCOORD2;
    float4 tangent : TEXCOORD3;
    float4 baseColorFactor : COLOR0;
    float4 flashColorAndAmount : COLOR1;
    float4 corruptionColorAndAmount : COLOR2;
    float4 corruptionParams : TEXCOORD4;
};

struct SceneData
{
    float4 cameraPosition;
    float4 sunlightDirection;
    float4 ambientColor;
    float4x4 lightViewProjection;
};

[[vk::binding(0, 0)]]
Sampler2D colorTexture;
[[vk::binding(1, 0)]]
Sampler2D normalTexture;
[[vk::binding(2, 0)]]
Sampler2D metallicRoughnessTexture;
[[vk::binding(3, 0)]]
Sampler2D occlusionTexture;
[[vk::binding(4, 0)]]
Sampler2D emissionTexture;

[[vk::binding(0, 1)]]
ConstantBuffer<SceneData> sceneData;

[[vk::binding(0, 2)]]
SamplerCube irradianceMap;
[[vk::binding(1, 2)]]
SamplerCube prefilteredMap;
[[vk::binding(2, 2)]]
Sampler2D brdfLUT;

[[vk::binding(0, 3)]]
Sampler2D shadowMap;

static const float PI = 3.14159265359;
static const float MIN_ROUGHNESS = 0.04;
static const float IBL_INTENSITY = 0.25;
static const float SHADOW_NORMAL_BIAS = 0.12;

float3 FresnelSchlick(float3 f0, float3 f90, float viewDotHalfway)
{
    return f0 + (f90 - f0) * pow(clamp(1.0 - viewDotHalfway, 0.0, 1.0), 5.0);
}

float GeometricOcclusion(float normalDotLight, float normalDotView, float alphaRoughness)
{
    float r = alphaRoughness;

    float attenuationLight =
        2.0 * normalDotLight /
        (normalDotLight + sqrt(r * r + (1.0 - r * r) * (normalDotLight * normalDotLight)));

    float attenuationView =
        2.0 * normalDotView /
        (normalDotView + sqrt(r * r + (1.0 - r * r) * (normalDotView * normalDotView)));

    return attenuationLight * attenuationView;
}

float MicrofacetDistribution(float normalDotHalfway, float alphaRoughness)
{
    float roughnessSquared = alphaRoughness * alphaRoughness;
    float f = (normalDotHalfway * roughnessSquared - normalDotHalfway) * normalDotHalfway + 1.0;
    return roughnessSquared / (PI * f * f);
}

float3 GetNormalFromMap(FragmentInput input)
{
    float3 tangentNormal =
        normalTexture.Sample(input.uv).xyz * 2.0 - 1.0;

    float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent.xyz);

    tangent = normalize(tangent - normal * dot(normal, tangent));

    float3 bitangent = cross(normal, tangent) * input.tangent.w;

    float3x3 tbn = float3x3(tangent, bitangent, normal);

    return normalize(mul(tbn, tangentNormal));
}

float3 ACESFilm(float3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;

    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

float Hash31(float3 p)
{
    p = frac(p * 0.1031);
    p += dot(p, p.yzx + 33.33);
    return frac((p.x + p.y) * p.z);
}

float BuildCorruptionMask(FragmentInput input)
{
    float amount = clamp(input.corruptionColorAndAmount.a, 0.0, 1.0);
    if (amount <= 0.0) {
        return 0.0;
    }

    float scale = max(input.corruptionParams.x, 0.001);
    float softness = clamp(input.corruptionParams.y, 0.001, 1.0);
    float time = input.corruptionParams.w;
    float3 scaledPosition = input.worldPosition * scale;

    float3 largeCell = floor(scaledPosition + float3(time * 0.37, 0.0, time * 0.19));
    float3 detailCell = floor(scaledPosition * 3.0 + float3(0.0, time * 1.7, 0.0));
    float cellNoise = Hash31(largeCell);
    float detailNoise = Hash31(detailCell);
    float stripeNoise = step(0.82, frac(dot(scaledPosition, float3(0.19, 0.73, 0.41)) + time * 2.3));
    float corruptionNoise = lerp(cellNoise, detailNoise, 0.35);
    corruptionNoise = lerp(corruptionNoise, stripeNoise, 0.22);

    float threshold = 1.0 - amount;
    return smoothstep(threshold - softness, threshold + softness, corruptionNoise);
}

float SampleShadow(FragmentInput input, float3 normal, float3 lightDirection)
{
    float3 shadowPosition = input.worldPosition + normal * SHADOW_NORMAL_BIAS;
    float4 lightClip = mul(sceneData.lightViewProjection, float4(shadowPosition, 1.0));
    float3 lightNdc = lightClip.xyz / lightClip.w;
    float2 shadowUv = lightNdc.xy * 0.5 + 0.5;

    if (shadowUv.x < 0.0 || shadowUv.x > 1.0 ||
        shadowUv.y < 0.0 || shadowUv.y > 1.0 ||
        lightNdc.z < 0.0 || lightNdc.z > 1.0) {
        return 1.0;
    }

    uint width;
    uint height;
    shadowMap.GetDimensions(width, height);
    float2 texelSize = 1.0 / float2(width, height);
    float bias = max(0.00002, 0.00015 * (1.0 - clamp(dot(normal, lightDirection), 0.0, 1.0)));
    float lit = 0.0;

    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            float closestReversedDepth = shadowMap.Sample(shadowUv + float2(x, y) * texelSize).r;
            lit += lightNdc.z + bias >= closestReversedDepth ? 1.0 : 0.0;
        }
    }

    return lit / 9.0;
}

[shader("fragment")]
float4 main(FragmentInput input) : SV_Target
{
    float4 baseColor = colorTexture.Sample(input.uv) * input.baseColorFactor;
    baseColor.rgb = lerp(
        baseColor.rgb,
        input.flashColorAndAmount.rgb,
        clamp(input.flashColorAndAmount.a, 0.0, 1.0));

    float4 metallicRoughness = metallicRoughnessTexture.Sample(input.uv);

    float roughness = metallicRoughness.g;
    float metallic = metallicRoughness.b;

    roughness = clamp(roughness, MIN_ROUGHNESS, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);

    float alphaRoughness = roughness * roughness;
    float ambientOcclusion = occlusionTexture.Sample(input.uv).r;
    float3 emissive = emissionTexture.Sample(input.uv).rgb;

    float3 n = normalize(input.normal);

    float3 light = normalize(sceneData.sunlightDirection.xyz);
    float3 view = normalize(sceneData.cameraPosition.xyz - input.worldPosition);
    float3 halfway = normalize(light + view);

    float normalDotLight = max(dot(n, light), 0.0);
    float normalDotView = clamp(abs(dot(n, view)), 0.001, 1.0);
    float normalDotHalfway = clamp(dot(n, halfway), 0.0, 1.0);
    float viewDotHalfway = clamp(dot(view, halfway), 0.0, 1.0);

    float3 f0 = float3(0.04);
    float3 diffuseColor = baseColor.rgb * (float3(1.0) - f0);
    diffuseColor *= 1.0 - metallic;

    float3 specularColor = lerp(f0, baseColor.rgb, metallic);

    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);

    float3 fresnel = FresnelSchlick(
        specularColor,
        float3(reflectance90),
        viewDotHalfway
    );

    float geometry = GeometricOcclusion(normalDotLight, normalDotView, alphaRoughness);
    float distribution = MicrofacetDistribution(normalDotHalfway, alphaRoughness);

    float3 diffuseContrib = (1.0 - fresnel) * (diffuseColor / PI);
    float3 specContrib = fresnel * geometry * distribution / max(4.0 * normalDotLight * normalDotView, 0.001);

    float3 lightColor =
        sceneData.ambientColor.rgb *
        sceneData.sunlightDirection.w;

    float3 ambient =
        baseColor.rgb *
        sceneData.ambientColor.rgb *
        sceneData.ambientColor.a;

    float shadow = normalDotLight > 0.0 ? SampleShadow(input, n, light) : 0.0;
    float3 color = ambient + shadow * normalDotLight * lightColor * (diffuseContrib + specContrib);
    color *= ambientOcclusion;

    color += emissive;

    float3 reflectionDirection = reflect(-view, n);

    float maxReflectionLod = 8.0;
    float reflectionLod =
        roughness * roughness * maxReflectionLod;

    float3 prefilteredColor =
        prefilteredMap.SampleLevel(reflectionDirection, reflectionLod).rgb;

    float2 brdf =
        brdfLUT.Sample(float2(normalDotView, roughness)).rg;

    float3 specularIBL =
        prefilteredColor *
        (fresnel * brdf.x + brdf.y);

    color += specularIBL;

    float3 irradiance =
        irradianceMap.Sample(n).rgb;

    float3 diffuseIBL =
        irradiance * diffuseColor * ambientOcclusion;

    color += diffuseIBL * IBL_INTENSITY;

    float corruptionMask = BuildCorruptionMask(input);
    if (corruptionMask > 0.0) {
        float3 corruptionColor =
            input.corruptionColorAndAmount.rgb *
            (1.0 + max(0.0, input.corruptionParams.z));
        color = lerp(color, corruptionColor, corruptionMask * input.corruptionColorAndAmount.a);
    }

    float exposure = 1.0;
    color *= exposure;
    color = ACESFilm(color);
    color = pow(color, float3(1.0 / 2.2));

    return float4(color, baseColor.a);
}
