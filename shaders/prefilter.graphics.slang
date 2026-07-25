struct PushConstants
{
    float4x4 viewProjection;
    float roughness;
};

[[vk::push_constant]]
PushConstants pushConstants;

static const float3 positions[36] = {
    float3(-1, -1, -1), float3(-1,  1, -1), float3( 1,  1, -1),
    float3( 1,  1, -1), float3( 1, -1, -1), float3(-1, -1, -1),

    float3(-1, -1,  1), float3( 1,  1,  1), float3(-1,  1,  1),
    float3( 1,  1,  1), float3(-1, -1,  1), float3( 1, -1,  1),

    float3(-1,  1,  1), float3( 1,  1,  1), float3( 1,  1, -1),
    float3( 1,  1, -1), float3(-1,  1, -1), float3(-1,  1,  1),

    float3(-1, -1,  1), float3(-1, -1, -1), float3( 1, -1, -1),
    float3( 1, -1, -1), float3( 1, -1,  1), float3(-1, -1,  1),

    float3( 1, -1,  1), float3( 1, -1, -1), float3( 1,  1, -1),
    float3( 1,  1, -1), float3( 1,  1,  1), float3( 1, -1,  1),

    float3(-1, -1,  1), float3(-1,  1, -1), float3(-1, -1, -1),
    float3(-1,  1, -1), float3(-1, -1,  1), float3(-1,  1,  1)
};

struct VertexOutput
{
    float4 position : SV_Position;
    float3 direction : TEXCOORD0;
};

struct FragmentInput
{
    float3 direction : TEXCOORD0;
};

[[vk::binding(0, 0)]]
SamplerCube environmentMap;

static const float PI = 3.14159265359;

[shader("vertex")]
VertexOutput vertexMain(uint vertexIndex : SV_VertexID)
{
    float3 position = positions[vertexIndex];

    VertexOutput output;
    output.direction = position;

    float4 clipPosition = mul(pushConstants.viewProjection, float4(position, 1.0));
    output.position = clipPosition.xyww;
    return output;
}

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

    float cosTheta =
        sqrt((1.0 - xi.y) /
        (1.0 + (a * a - 1.0) * xi.y));

    float sinTheta =
        sqrt(1.0 - cosTheta * cosTheta);

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

    float3 sampleVector =
        tangent * halfway.x +
        bitangent * halfway.y +
        normal * halfway.z;

    return normalize(sampleVector);
}

[shader("fragment")]
float4 fragmentMain(FragmentInput input) : SV_Target
{
    float3 normal = normalize(input.direction);

    float3 reflection = normal;
    float3 view = reflection;

    const uint SAMPLE_COUNT = 1024u;

    float3 prefilteredColor = float3(0.0);
    float totalWeight = 0.0;

    for (uint i = 0u; i < SAMPLE_COUNT; i++) {
        float2 xi = Hammersley(i, SAMPLE_COUNT);

        float3 halfway =
            ImportanceSampleGGX(xi, normal, pushConstants.roughness);

        float3 light =
            normalize(2.0 * dot(view, halfway) * halfway - view);

        float normalDotLight =
            max(dot(normal, light), 0.0);

        if (normalDotLight > 0.0) {
            prefilteredColor +=
                environmentMap.Sample(light).rgb * normalDotLight;

            totalWeight += normalDotLight;
        }
    }

    prefilteredColor =
        prefilteredColor / max(totalWeight, 0.001);

    return float4(prefilteredColor, 1.0);
}
