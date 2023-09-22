#include "TransformComponent.hpp"

TransformComponent::TransformComponent()
    : position(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }),
    rotation(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }),
    scale(DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f })
{
}

TransformComponent::TransformComponent(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const DirectX::XMFLOAT3& scale)
    : position(pos), rotation(rot), scale(scale)
{
}

DirectX::XMMATRIX TransformComponent::GetWorldMatrix() const
{
    DirectX::XMMATRIX positionMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    return scaleMatrix * rotationMatrix * positionMatrix;
}

std::ostream& operator<<(std::ostream& os, const TransformComponent& transform)
{
    os << "Position: (" << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << ")";
    os << " Rotation: (" << transform.rotation.x << ", " << transform.rotation.y << ", " << transform.rotation.z << ")";
    os << " Scale: (" << transform.scale.x << ", " << transform.scale.y << ", " << transform.scale.z << ")";
    return os;
}