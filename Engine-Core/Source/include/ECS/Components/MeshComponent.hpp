#pragma once
#include <vector>
#include <DirectXMath.h>

#include "Definitions.hpp"
#include "VertexType.hpp"
#include "ComponentIdentifier.hpp"
#include "Component.hpp"
#include <Heightmap.hpp>


enum class PrimitiveGeometryType3D
{
	Cube,
	Sphere,
	Cylinder,
	Pipe,
	TerrainChunk, // TODO REMOVE FROM HERE ONLY FOR TESTING,
    Skybox
};

class IMeshComponent
{
public:
    virtual ~IMeshComponent() = default;
    virtual bool IsDirty() const = 0;
    virtual void SetIsDirty(bool isDirty) = 0;
    virtual int GetInstancePoolIndex() const = 0;
    virtual void SetInstancePoolIndex(int index) = 0;
};

template <typename TVertex>
class MeshComponent : public IMeshComponent, public Component
{
	std::vector<TVertex> _vertices;
	std::vector<UINT> _indices;
	int _instancePoolIndex;
	// Engine models use prefix Engine:: ?? maybe, I don't know yet what the json will look like
	std::string _path;
	bool _isDirty = true; // TODO find a common system for dirtyness
public:
	MeshComponent() {}

	MeshComponent(const std::vector<TVertex>& vertices, const std::vector<UINT>& indices) :
		_vertices(vertices), _indices(indices)
	{
	}

	MeshComponent(
		const std::vector<TVertex>& vertices,
		const std::vector<UINT>& indices, int instancePoolIndex) :
		_vertices(vertices), _indices(indices), _instancePoolIndex(instancePoolIndex)
	{
	}

	void SetVertices(const std::vector<TVertex>& vertices)
	{
		_vertices = vertices;
		_isDirty = true;
	}

	void SetIndices(const std::vector<UINT>& indices)
	{
		_indices = indices;
		_isDirty = true;
	}


	void SetInstancePoolIndex(int id) override
	{
		_instancePoolIndex = id;
	}

	void SetPath(std::string path)
	{
		_path = path;
	}

	std::vector<TVertex> GetVertices() const
	{
		return _vertices;
	}

	std::vector<UINT> GetIndices() const
	{
		return _indices;
	}

	int GetInstancePoolIndex() const override
	{
		return _instancePoolIndex;
	}

	bool IsDirty() const override
	{
		return _isDirty;
	}

	void SetIsDirty(bool isDirty) override
	{
		_isDirty = isDirty;
	}

    static MeshComponent<VertexPositionNormalUv> GeneratePrimitiveMeshComponent(PrimitiveGeometryType3D type)
    {
        MeshComponent<VertexPositionNormalUv> mesh = MeshComponent<VertexPositionNormalUv>(GetPrimitiveMeshVertices(type), GetPrimitiveMeshIndices(type));
        mesh.SetInstancePoolIndex((int)type);
        return mesh;
    }

    static std::vector<VertexPositionNormalUv> GetPrimitiveMeshVertices(PrimitiveGeometryType3D type)
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
            case PrimitiveGeometryType3D::Skybox:
                return GetPrimitiveCubeVertices();
                //case PrimitiveGeometryType3D::TerrainChunk:
                    //return GetPrimitiveTerrainChunkVertices(10.0f, 10.0f, 10, 10);
        }
    }

    static MeshComponent<VertexPositionNormalUv> GenerateTerrainMeshComponent(PrimitiveGeometryType3D type, const Heightmap& heightmap)
    {
        MeshComponent<VertexPositionNormalUv> mesh = MeshComponent<VertexPositionNormalUv>(GetTerrainMeshVertices(heightmap), GetTerrainMeshIndices());
        mesh.SetInstancePoolIndex((int)type);
        return mesh;
    }

    static std::vector<VertexPositionNormalUv> GetTerrainMeshVertices(const Heightmap& heightmap)
    {
        return GetPrimitiveTerrainChunkVertices(256.0f, 256.0f, heightmap);
    }

    static std::vector<UINT> GetTerrainMeshIndices()
    {
        return GetPrimitiveTerrainChunkIndices(256.0f, 256.0f, 256, 256);
    }

    static std::vector<UINT> GetPrimitiveMeshIndices(PrimitiveGeometryType3D type)
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
        case PrimitiveGeometryType3D::Skybox:
            return GetPrimitiveSkyboxIndices();
            //case PrimitiveGeometryType3D::TerrainChunk:
                //return GetPrimitiveTerrainChunkIndices(10.0f, 10.0f, 10, 10);
        }
    }

    static std::vector<VertexPositionNormalUv> GetPrimitiveCubeVertices()
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

    static std::vector<VertexPositionNormalUv> GetPrimitiveSphereVertices(float radius, int numSlices, int numStacks)
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

    static std::vector<VertexPositionNormalUv> GetPrimitiveCylinderVertices(float radius, float height, int numSlices)
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

    static std::vector<VertexPositionNormalUv> GetPrimitivePipeVertices(float radius, float height, int numSlices)
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

    static std::vector<VertexPositionNormalUv> GetPrimitiveTerrainChunkVertices(
        int densityX,
        int densityY,
        const Heightmap& heightmap
    )
    {
        auto width = heightmap.GetWidth();
        auto height = heightmap.GetHeight();
        float horizontalGap = width / densityX;
        float verticalGap = height / densityY;

        float halfWidth = width / 2;
        float halfHeight = height / 2;

        std::vector<VertexPositionNormalUv> vertices;
        for (float y = 0.0f; y < height; y += verticalGap)
        {

            for (float x = 0.0f; x < width; x += horizontalGap)
            {
                float u = x / width;
                float v = y / height;
                vertices.push_back({
                    {x - halfWidth, 0, y - halfHeight}, // Position
                    {0.0f, 1.0f, 0.0f},                 // Normal
                    {u, v}                              //UV
                });
            }
        }

        return vertices;
    }

    static std::vector<UINT> GetPrimitiveCubeIndices()
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

    static std::vector<UINT> GetPrimitiveSkyboxIndices()
    {
        return {
            // Front
            0, 1, 2,
            2, 1, 3,

            // Back
            6, 5, 4,
            6, 7, 5,

            // Top
            8, 9, 10,
            10, 9, 11,

            // Bottom
            12, 14, 13,
            14, 15, 13,

            // Left
            16, 17, 18,
            18, 17, 19,

            // Right
            20, 22, 21,
            22, 23, 21,
        };
    }

    static std::vector<UINT> GetPrimitiveSphereIndices(int numSlices, int numStacks)
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

    static std::vector<UINT> GetPrimitiveCylinderIndices(int numSlices)
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

    static std::vector<UINT> GetPrimitivePipeIndices(int numSlices)
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

    static std::vector<UINT> GetPrimitiveTerrainChunkIndices(float width, float height, int densityX, int densityY)
    {
        // 0 - 1
        // | / |
        // 2   3
        std::vector<UINT> indices;

        for (int y = 0; y < densityY - 1; y++) {
            for (int x = 0; x < densityX - 1; x++) {
                UINT topLeft = y * densityX + x;
                UINT topRight = topLeft + 1;
                UINT bottomLeft = (y + 1) * densityX + x;
                UINT bottomRight = bottomLeft + 1;

                indices.push_back(topLeft);
                indices.push_back(topRight);
                indices.push_back(bottomLeft);

                indices.push_back(topRight);
                indices.push_back(bottomRight);
                indices.push_back(bottomLeft);
            }
        }
        return indices;
    }

    bool FinalizeLoading()
    {
        if (_path != "")
        {
            // load the model
            return true;
        }

        _vertices = MeshComponent::GetPrimitiveMeshVertices((PrimitiveGeometryType3D)_instancePoolIndex);
        _indices = MeshComponent::GetPrimitiveMeshIndices((PrimitiveGeometryType3D)_instancePoolIndex);
        return true;
    }


	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(CEREAL_NVP(_instancePoolIndex), CEREAL_NVP(_path));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(CEREAL_NVP(_instancePoolIndex), CEREAL_NVP(_path));
		FinalizeLoading();
	}
};

