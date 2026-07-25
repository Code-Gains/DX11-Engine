struct FragmentInput
{
    float3 direction : TEXCOORD0;
};

[[vk::binding(0, 0)]]
SamplerCube environmentMap;

static const float PI = 3.14159265359;

[shader("fragment")]
float4 main(FragmentInput input) : SV_Target
{
    float3 normal = normalize(input.direction);

    float3 up = float3(0.0, 1.0, 0.0);
    float3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    float3 irradiance = float3(0.0);

    float sampleDelta = 0.025;
    float sampleCount = 0.0;

    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            float3 tangentSample = float3(
                sin(theta) * cos(phi),
                sin(theta) * sin(phi),
                cos(theta)
            );

            float3 sampleVector =
                tangentSample.x * right +
                tangentSample.y * up +
                tangentSample.z * normal;

            irradiance += environmentMap.Sample(sampleVector).rgb *
                cos(theta) * sin(theta);

            sampleCount++;
        }
    }

    irradiance = PI * irradiance / sampleCount;

    return float4(irradiance, 1.0);
}
