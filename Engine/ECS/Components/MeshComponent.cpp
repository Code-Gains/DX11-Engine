#include "MeshComponent.hpp"

MeshComponent::MeshComponent() : ComponentIdentifier(0)
{
}

MeshComponent::MeshComponent(const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices) :
	_vertices(vertices), _indices(indices), ComponentIdentifier(0)
{
}

MeshComponent::MeshComponent(int id, const std::vector<VertexPositionNormalUv>& vertices, const std::vector<UINT>& indices) :
	_vertices(vertices), _indices(indices), ComponentIdentifier(id)
{
}

MeshComponent::MeshComponent(
	int id,
	const std::vector<VertexPositionNormalUv>& vertices,
	const std::vector<UINT>& indices, int instancePoolIndex) : 
	_vertices(vertices), _indices(indices), _instancePoolIndex(instancePoolIndex), ComponentIdentifier(id)
{
}

void MeshComponent::SetVertices(const std::vector<VertexPositionNormalUv>& vertices)
{
	_vertices = vertices;
}

void MeshComponent::SetIndices(const std::vector<UINT>& indices)
{
	_indices = indices;
}

void MeshComponent::SetComponentIdentifier(int id)
{
	SetId(id);
}

void MeshComponent::SetInstancePoolIndex(int id)
{
	_instancePoolIndex = id;
}

std::vector<VertexPositionNormalUv> MeshComponent::GetVertices() const
{
	return _vertices;
}

std::vector<UINT> MeshComponent::GetIndices() const
{
	return _indices;
}

int MeshComponent::GetInstancePoolIndex() const
{
	return _instancePoolIndex;
}

MeshComponent MeshComponent::GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D type)
{
    MeshComponent mesh = MeshComponent(GetVertices(type), GetIndices(type));
    mesh.SetInstancePoolIndex((int)type);
    return mesh;
}

std::vector<VertexPositionNormalUv> MeshComponent::GetVertices(PrimitiveGeometryType3D type)
{
    switch (type)
    {
        case PrimitiveGeometryType3D::Cube:
            return GetPrimitiveCubeVertices();
        case PrimitiveGeometryType3D::Sphere:
            return GetPrimitiveSphereVertices(0.5f, 30, 30);
        case PrimitiveGeometryType3D::Cylinder:
            return GetPrimitiveCylinderVertices(0.5f, 1, 30);
        case PrimitiveGeometryType3D::Pipe:
            return GetPrimitivePipeVertices(0.5f, 1, 30);
    }
}

std::vector<UINT> MeshComponent::GetIndices(PrimitiveGeometryType3D type)
{
    switch (type)
    {
    case PrimitiveGeometryType3D::Cube:
        return GetPrimitiveCubeIndices();
    case PrimitiveGeometryType3D::Sphere:
        return GetPrimitiveSphereIndices(30, 30);
    case PrimitiveGeometryType3D::Cylinder:
        return GetPrimitiveCylinderIndices(30);
    case PrimitiveGeometryType3D::Pipe:
        return GetPrimitivePipeIndices(30);
    }
}

std::vector<VertexPositionNormalUv> MeshComponent::GetPrimitiveCubeVertices()
{
    return std::vector<VertexPositionNormalUv> {
        // Front
        VertexPositionNormalUv{ Position{ -0.5f, -0.5f, 0.5f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{0.0f, 1.0f} },
            VertexPositionNormalUv{ Position{  0.5f, -0.5f, 0.5f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{1.0f, 1.0f} },
            VertexPositionNormalUv{ Position{ -0.5f,  0.5f, 0.5f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{0.0f, 0.0f} },
            VertexPositionNormalUv{ Position{  0.5f,  0.5f, 0.5f }, Normal{ 0.0f, 0.0f, 1.0f }, Uv{1.0f, 0.0f} },

            // Back
            VertexPositionNormalUv{ Position{ -0.5f, -0.5f, -0.5f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{0.0f, 1.0f} },
            VertexPositionNormalUv{ Position{  0.5f, -0.5f, -0.5f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{1.0f, 1.0f} },
            VertexPositionNormalUv{ Position{ -0.5f,  0.5f, -0.5f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{0.0f, 0.0f} },
            VertexPositionNormalUv{ Position{  0.5f,  0.5f, -0.5f }, Normal{ 0.0f, 0.0f, -1.0f }, Uv{1.0f, 0.0f} },

            // Top
            VertexPositionNormalUv{ Position{ -0.5f, 0.5f, 0.5f },  Normal{  0.0f, 1.0f, 0.0f }, Uv{0.0f, 1.0f} },
            VertexPositionNormalUv{ Position{  0.5f, 0.5f, 0.5f },  Normal{  0.0f, 1.0f, 0.0f }, Uv{1.0f, 1.0f} },
            VertexPositionNormalUv{ Position{ -0.5f, 0.5f, -0.5f }, Normal{ 0.0f, 1.0f, 0.0f },  Uv{0.0f, 0.0f} },
            VertexPositionNormalUv{ Position{  0.5f, 0.5f, -0.5f }, Normal{ 0.0f, 1.0f, 0.0f },  Uv{1.0f, 0.0f} },

            // Bottom
            VertexPositionNormalUv{ Position{ -0.5f, -0.5f, 0.5f },  Normal{  0.0f, -1.0f, 0.0f }, Uv{0.0f, 1.0f} },
            VertexPositionNormalUv{ Position{  0.5f, -0.5f, 0.5f },  Normal{  0.0f, -1.0f, 0.0f }, Uv{1.0f, 1.0f} },
            VertexPositionNormalUv{ Position{ -0.5f, -0.5f, -0.5f }, Normal{ 0.0f, -1.0f, 0.0f },  Uv{0.0f, 0.0f} },
            VertexPositionNormalUv{ Position{  0.5f, -0.5f, -0.5f }, Normal{ 0.0f, -1.0f, 0.0f },  Uv{1.0f, 0.0f} },

            // Left
            VertexPositionNormalUv{ Position{ -0.5f, -0.5f, -0.5f }, Normal{ -1.0f, 0.0f, 0.0f },  Uv{0.0f, 1.0f} },
            VertexPositionNormalUv{ Position{ -0.5f, -0.5f, 0.5f },  Normal{  -1.0f, 0.0f, 0.0f }, Uv{1.0f, 1.0f} },
            VertexPositionNormalUv{ Position{ -0.5f,  0.5f, -0.5f }, Normal{ -1.0f, 0.0f, 0.0f },  Uv{0.0f, 0.0f} },
            VertexPositionNormalUv{ Position{ -0.5f,  0.5f, 0.5f },  Normal{  -1.0f, 0.0f, 0.0f }, Uv{1.0f, 0.0f} },

            // Right
            VertexPositionNormalUv{ Position{ 0.5f, -0.5f, -0.5f }, Normal{ 1.0f, 0.0f, 0.0f },  Uv{0.0f, 1.0f} },
            VertexPositionNormalUv{ Position{ 0.5f, -0.5f, 0.5f },  Normal{  1.0f, 0.0f, 0.0f }, Uv{1.0f, 1.0f} },
            VertexPositionNormalUv{ Position{ 0.5f,  0.5f, -0.5f }, Normal{ 1.0f, 0.0f, 0.0f },  Uv{0.0f, 0.0f} },
            VertexPositionNormalUv{ Position{ 0.5f,  0.5f, 0.5f },  Normal{  1.0f, 0.0f, 0.0f }, Uv{1.0f, 0.0f} },
    };
}

std::vector<VertexPositionNormalUv> MeshComponent::GetPrimitiveSphereVertices(float radius, int numSlices, int numStacks)
{
    std::vector<VertexPositionNormalUv> vertices;

    // Iterate over latitude and longitude
    for (int i = 0; i <= numStacks; ++i) {
        float theta = i * DirectX::XM_PI / numStacks;          // Range [0, PI]
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int j = 0; j <= numSlices; ++j) {
            float phi = j * 2.0f * DirectX::XM_PI / numSlices; // Range [0, 2PI]
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            // Convert polar coordinates to Cartesian (x, y, z)
            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            // Compute the texture coordinates
            float u = 1.0f - (float)j / numSlices;
            float v = 1.0f - (float)i / numStacks;

            vertices.push_back({
                {x * radius, y * radius, z * radius}, // Position
                {x, y, z},                            // Normal
                {u, v}                                // Uv
                });
        }
    }
    return vertices;
}

std::vector<VertexPositionNormalUv> MeshComponent::GetPrimitiveCylinderVertices(float radius, float height, int numSlices)
{
    std::vector<VertexPositionNormalUv> vertices;

    // Generate cap vertices and side vertices
    for (int i = 0; i <= numSlices; ++i) {
        float phi = i * 2.0f * DirectX::XM_PI / numSlices;
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);

        // Top cap
        vertices.push_back({
            {radius * cosPhi, height / 2, radius * sinPhi},  // Position
            {0, 1, 0},                                       // Normal
            {cosPhi * 0.5f + 0.5f, sinPhi * 0.5f + 0.5f}     // Uv
            });

        // Bottom cap
        vertices.push_back({
            {radius * cosPhi, -height / 2, radius * sinPhi}, // Position
            {0, -1, 0},                                      // Normal
            {cosPhi * 0.5f + 0.5f, sinPhi * 0.5f + 0.5f}     // Uv
            });

        // Side top
        vertices.push_back({
            {radius * cosPhi, height / 2, radius * sinPhi},  // Position
            {cosPhi, 0, sinPhi},                             // Normal
            {(float)i / numSlices, 1.0f}                      // Uv
            });

        // Side bottom
        vertices.push_back({
            {radius * cosPhi, -height / 2, radius * sinPhi}, // Position
            {cosPhi, 0, sinPhi},                             // Normal
            {(float)i / numSlices, 0.0f}                      // Uv
            });
    }

    // Top center vertex
    vertices.push_back({
        {0, height / 2, 0},  // Position
        {0, 1, 0},           // Normal
        {0.5f, 0.5f}         // UV
        });

    // Bottom center vertex
    vertices.push_back({
        {0, -height / 2, 0}, // Position
        {0, -1, 0},          // Normal
        {0.5f, 0.5f}         // UV
        });

    return vertices;
}

std::vector<VertexPositionNormalUv> MeshComponent::GetPrimitivePipeVertices(float radius, float height, int numSlices)
{
    std::vector<VertexPositionNormalUv> vertices;

    // Generate cap vertices and side vertices
    for (int i = 0; i <= numSlices; ++i) {
        float phi = i * 2.0f * DirectX::XM_PI / numSlices;
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);

        // Side top
        vertices.push_back({
            {radius * cosPhi, height / 2, radius * sinPhi},  // Position
            {cosPhi, 0, sinPhi},                             // Normal
            {(float)i / numSlices, 1.0f}                      // Uv
            });

        // Side bottom
        vertices.push_back({
            {radius * cosPhi, -height / 2, radius * sinPhi}, // Position
            {cosPhi, 0, sinPhi},                             // Normal
            {(float)i / numSlices, 0.0f}                      // Uv
            });
    }

    return vertices;
}

std::vector<UINT> MeshComponent::GetPrimitiveCubeIndices()
{
    return {
        // Front
        0, 2, 1,
        2, 3, 1,

        // Back
        6, 4, 5,
        6, 5, 7,

        // Top
        8, 10, 9,
        10, 11, 9,

        // Bottom
        12, 13, 14,
        14, 13, 15,

        // Left
        16, 18, 17,
        18, 19, 17,

        // Right
        20, 21, 22,
        22, 21, 23,
    };
}

std::vector<UINT> MeshComponent::GetPrimitiveSphereIndices(int numSlices, int numStacks)
{
    std::vector<UINT> indices;

    for (int i = 0; i < numStacks; ++i) {
        for (int j = 0; j < numSlices; ++j) {
            int first = (i * (numSlices + 1)) + j;
            int second = first + numSlices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return indices;
}

std::vector<UINT> MeshComponent::GetPrimitiveCylinderIndices(int numSlices)
{
    std::vector<UINT> indices;
    int vertexOffset = 4;

    for (int i = 0; i < numSlices; ++i) {
        // Side
        indices.push_back(i * vertexOffset + 2);
        indices.push_back(i * vertexOffset + 3);
        indices.push_back(((i + 1) % numSlices) * vertexOffset + 2);

        indices.push_back(((i + 1) % numSlices) * vertexOffset + 2);
        indices.push_back(i * vertexOffset + 3);
        indices.push_back(((i + 1) % numSlices) * vertexOffset + 3);

        int topCenterIndex = numSlices * vertexOffset;
        int bottomCenterIndex = numSlices * vertexOffset + 1;

        // Top cap
        indices.push_back(i * 4);
        indices.push_back(((i + 1) % numSlices) * 4);
        indices.push_back(topCenterIndex);

        // Bottom cap
        indices.push_back(i * 4 + 1);
        indices.push_back(bottomCenterIndex);
        indices.push_back(((i + 1) % numSlices) * 4 + 1);
    }

    return indices;
}

std::vector<UINT> MeshComponent::GetPrimitivePipeIndices(int numSlices)
{
    std::vector<UINT> indices;
    int vertexOffset = 2;

    for (int i = 0; i < numSlices; ++i) {
        // Side
        indices.push_back(i * vertexOffset + 2);
        indices.push_back(i * vertexOffset + 3);
        indices.push_back(((i + 1) % numSlices) * vertexOffset + 2);

        indices.push_back(((i + 1) % numSlices) * vertexOffset + 2);
        indices.push_back(i * vertexOffset + 3);
        indices.push_back(((i + 1) % numSlices) * vertexOffset + 3);
    }

    return indices;
}
