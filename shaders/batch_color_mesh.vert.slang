struct Vertex
{
    float3 position;
    float uv_x;
    float3 normal;
    float uv_y;
    float4 tangent;
};

struct InstanceData
{
    float3 position;
    float pad0;
    float4 rotation;
    float3 scale;
    float pad1;
    float4 baseColorFactor;
};

struct PushConstants
{
    float4x4 viewProjection;
    float4 baseColorFactor;
    uint64_t vertexBuffer;
    uint64_t instanceBuffer;
};

[[vk::push_constant]]
PushConstants pushConstants;

struct VertexOutput
{
    float4 position : SV_Position;
    float3 worldPosition : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float2 uv : TEXCOORD2;
    float4 tangent : TEXCOORD3;
    float4 baseColorFactor : COLOR0;
};

float4x4 BuildTransform(float3 position, float4 q, float3 scale)
{
    float x2 = q.x * q.x;
    float y2 = q.y * q.y;
    float z2 = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    return float4x4(
        scale.x * (1.0 - 2.0 * (y2 + z2)), scale.x * 2.0 * (xy + wz),          scale.x * 2.0 * (xz - wy),          0.0,
        scale.y * 2.0 * (xy - wz),          scale.y * (1.0 - 2.0 * (x2 + z2)), scale.y * 2.0 * (yz + wx),          0.0,
        scale.z * 2.0 * (xz + wy),          scale.z * 2.0 * (yz - wx),          scale.z * (1.0 - 2.0 * (x2 + y2)), 0.0,
        position.x,                         position.y,                         position.z,                         1.0
    );
}

float3 ApplyTransform(float3 point, float3 position, float4 quat, float3 scale)
{
    point *= scale;

    float3 t = 2.0 * cross(quat.xyz, point);
    point += quat.w * t + cross(quat.xyz, t);
    return point + position;
}

float3x3 InverseTranspose3x3(float3x3 matrix)
{
    float3 row0 = matrix[0];
    float3 row1 = matrix[1];
    float3 row2 = matrix[2];

    float3 cof0 = cross(row1, row2);
    float3 cof1 = cross(row2, row0);
    float3 cof2 = cross(row0, row1);

    float invDet = 1.0 / dot(row0, cof0);
    return float3x3(cof0 * invDet, cof1 * invDet, cof2 * invDet);
}

[shader("vertex")]
VertexOutput main(uint vertexIndex : SV_VertexID, uint instanceIndex : SV_InstanceID)
{
    Ptr<Vertex> vertices = reinterpret<Ptr<Vertex>>(pushConstants.vertexBuffer);
    Ptr<InstanceData> instances = reinterpret<Ptr<InstanceData>>(pushConstants.instanceBuffer);

    Vertex vertex = vertices[vertexIndex];
    InstanceData instance = instances[instanceIndex];

    float3 worldPosition = ApplyTransform(vertex.position, instance.position, instance.rotation, instance.scale);
    float3x3 model3x3 = (float3x3)BuildTransform(instance.position, instance.rotation, instance.scale);
    float3x3 normalMatrix = InverseTranspose3x3(model3x3);

    VertexOutput output;
    output.position = mul(pushConstants.viewProjection, float4(worldPosition, 1.0));
    output.worldPosition = worldPosition;
    output.normal = normalize(mul(normalMatrix, vertex.normal));
    output.uv = float2(vertex.uv_x, vertex.uv_y);
    output.tangent = float4(normalize(mul(model3x3, vertex.tangent.xyz)), vertex.tangent.w);
    output.baseColorFactor = pushConstants.baseColorFactor * instance.baseColorFactor;
    return output;
}
