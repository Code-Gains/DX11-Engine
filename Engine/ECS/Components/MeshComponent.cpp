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

MeshComponent MeshComponent::GetPrimitiveMeshComponent(PrimitiveGeometryType3D type)
{
	return GetPrimitiveMeshComponent(0, type);
}

MeshComponent MeshComponent::GetPrimitiveMeshComponent(int id, PrimitiveGeometryType3D type)
{
	switch (type)
	{
		case PrimitiveGeometryType3D::Cube:
		{
			auto cube = Cube();
			auto meshComponent = MeshComponent(id, cube.GetVertices(), cube.GetIndices());
			return meshComponent;
		}
		case PrimitiveGeometryType3D::Sphere:
		{
			auto sphere = Sphere();
			auto meshComponent = MeshComponent(id, sphere.GetVertices(), sphere.GetIndices());
			return meshComponent;
		}
		case PrimitiveGeometryType3D::Cylinder:
		{
			auto cylinder = Cylinder(true);
			auto meshComponent = MeshComponent(id, cylinder.GetVertices(), cylinder.GetIndices());
			return meshComponent;
		}
		case PrimitiveGeometryType3D::Pipe:
		{
			auto pipe = Cylinder(false);
			auto meshComponent = MeshComponent(id, pipe.GetVertices(), pipe.GetIndices());
			return meshComponent;
		}
		default:
			return MeshComponent();
	}
}
