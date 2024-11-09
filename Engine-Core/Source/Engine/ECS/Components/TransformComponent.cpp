#include "TransformComponent.hpp"

TransformComponent::TransformComponent() :
    _position(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }),
    _rotation(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }),
    _scale(DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f })
{
    ComponentRegistry::RegisterComponentUI<TransformComponent, TransformComponentUI>();
}

TransformComponent::TransformComponent(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const DirectX::XMFLOAT3& scale)
    : _position(pos), _rotation(rot), _scale(scale)
{
}

void TransformComponent::SetPosition(const DirectX::XMFLOAT3& position)
{
    _position = position;
    _isDirty = true;
}

void TransformComponent::SetRotation(const DirectX::XMFLOAT3& rotation)
{
    _rotation = rotation;
    _isDirty = true;
}

void TransformComponent::SetScale(const DirectX::XMFLOAT3& scale)
{
    _scale = scale;
    _isDirty = true;
}

DirectX::XMFLOAT3 TransformComponent::GetPosition() const
{
    return _position;
}

DirectX::XMFLOAT3 TransformComponent::GetRotation() const
{
    return _rotation;
}

DirectX::XMFLOAT3 TransformComponent::GetScale() const
{
    return _scale;
}

DirectX::XMFLOAT3& TransformComponent::GetPositionByRef()
{
    return _position;
}

DirectX::XMFLOAT3& TransformComponent::GetRotationByRef()
{
    return _rotation;
}

DirectX::XMFLOAT3& TransformComponent::GetScaleByRef()
{
    return _scale;
}

DirectX::XMMATRIX TransformComponent::GetWorldMatrix() const
{
    DirectX::XMMATRIX positionMatrix = DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z);
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z);
    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(_scale.x, _scale.y, _scale.z);

    return scaleMatrix * rotationMatrix * positionMatrix;
}

bool TransformComponent::IsDirty() const
{
    return _isDirty;
}

void TransformComponent::SetIsDirty(bool isDirty)
{
    _isDirty = isDirty;
}

std::ostream& operator<<(std::ostream& os, const TransformComponent& transform)
{
    os << "Position: (" << transform.GetPosition().x << ", " << transform.GetPosition().y << ", " << transform.GetPosition().z << ")";
    os << " Rotation: (" << transform.GetRotation().x << ", " << transform.GetRotation().y << ", " << transform.GetRotation().z << ")";
    os << " Scale: (" << transform.GetScale().x << ", " << transform.GetScale().y << ", " << transform.GetScale().z << ")";
    return os;
}

void TransformComponentUI::RenderUI(Component& component)
{
    // Cast to actual component type
    auto& transform = static_cast<TransformComponent&>(component);

    // Get Fields
    auto& selectedPosition = transform.GetPositionByRef();
    auto& selectedRotation = transform.GetRotationByRef();
    auto& selectedScale = transform.GetScaleByRef();

    // Render fields and allow to set new values
    if (ImGui::DragFloat3("Position", &selectedPosition.x, 0.1f))
    {
        transform.SetPosition(selectedPosition);
    }
    if (ImGui::DragFloat3("Rotation", &selectedRotation.x, 0.1f))
    {
        transform.SetRotation(selectedRotation);
    }
    if (ImGui::DragFloat3("Scale", &selectedScale.x, 0.1f))
    {
        transform.SetScale(selectedScale);
    }
}
