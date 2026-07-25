struct FragmentInput
{
    float2 uv : TEXCOORD0;
};

static const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

    return float(bits) * 2.3283064365386963e-10;
}

float2 Hammersley(uint i, uint sampleCount)
{
    return float2(float(i) / float(sampleCount), RadicalInverse_VdC(i));
}

float3 ImportanceSampleGGX(float2 xi, float3 normal, float roughness)
{
    float a = roughness * roughness;

    float phi = 2.0 * PI * xi.x;
    float cosTheta = sqrt(
        (1.0 - xi.y) /
        (1.0 + (a * a - 1.0) * xi.y)
    );
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    float3 halfway;
    halfway.x = cos(phi) * sinTheta;
    halfway.y = sin(phi) * sinTheta;
    halfway.z = cosTheta;

    float3 up =
        abs(normal.z) < 0.999
        ? float3(0.0, 0.0, 1.0)
        : float3(1.0, 0.0, 0.0);

    float3 tangent = normalize(cross(up, normal));
    float3 bitangent = cross(normal, tangent);

    return normalize(
        tangent * halfway.x +
        bitangent * halfway.y +
        normal * halfway.z
    );
}

float GeometrySchlickGGX(float normalDotView, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float numerator = normalDotView;
    float denominator = normalDotView * (1.0 - k) + k;

    return numerator / denominator;
}

float GeometrySmith(float3 normal, float3 view, float3 light, float roughness)
{
    float normalDotView = max(dot(normal, view), 0.0);
    float normalDotLight = max(dot(normal, light), 0.0);

    float ggx1 = GeometrySchlickGGX(normalDotView, roughness);
    float ggx2 = GeometrySchlickGGX(normalDotLight, roughness);

    return ggx1 * ggx2;
}

float2 IntegrateBRDF(float normalDotView, float roughness)
{
    float3 view;
    view.x = sqrt(1.0 - normalDotView * normalDotView);
    view.y = 0.0;
    view.z = normalDotView;

    float a = 0.0;
    float b = 0.0;

    float3 normal = float3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;

    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        float2 xi = Hammersley(i, SAMPLE_COUNT);

        float3 halfway = ImportanceSampleGGX(xi, normal, roughness);
        float3 light = normalize(2.0 * dot(view, halfway) * halfway - view);

        float normalDotLight = max(light.z, 0.0);
        float normalDotHalfway = max(halfway.z, 0.0);
        float viewDotHalfway = max(dot(view, halfway), 0.0);

        if (normalDotLight > 0.0) {
            float geometry = GeometrySmith(normal, view, light, roughness);
            float geometryVisibility =
                (geometry * viewDotHalfway) /
                (normalDotHalfway * normalDotView);

            float fresnel = pow(1.0 - viewDotHalfway, 5.0);

            a += (1.0 - fresnel) * geometryVisibility;
            b += fresnel * geometryVisibility;
        }
    }

    a /= float(SAMPLE_COUNT);
    b /= float(SAMPLE_COUNT);

    return float2(a, b);
}

[shader("fragment")]
float2 main(FragmentInput input) : SV_Target
{
    float normalDotView = clamp(input.uv.x, 0.001, 1.0);
    float roughness = clamp(input.uv.y, 0.001, 1.0);

    return IntegrateBRDF(normalDotView, roughness);
}
