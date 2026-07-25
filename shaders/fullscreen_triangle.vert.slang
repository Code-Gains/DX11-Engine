struct VertexOutput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

[shader("vertex")]
VertexOutput main(uint vertexIndex : SV_VertexID)
{
    float2 position = float2(
        vertexIndex == 2 ? 3.0 : -1.0,
        vertexIndex == 1 ? 3.0 : -1.0
    );

    VertexOutput output;
    output.position = float4(position, 0.0, 1.0);
    output.uv = position * 0.5 + 0.5;
    return output;
}
