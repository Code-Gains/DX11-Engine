#pragma once
#include <DirectXMath.h>
#include <cereal/cereal.hpp>
#include "ComponentIdentifier.hpp"
#include "ComponentRegistry.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

struct MaterialConstantBuffer
{
    DirectX::XMFLOAT4 ambient;
    DirectX::XMFLOAT4 diffuse;
    DirectX::XMFLOAT4 specular;
    float shininess;
    float padding[3] = { 0, 0, 0 };

    MaterialConstantBuffer();
    MaterialConstantBuffer(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, float shininess);
};

class MaterialComponent : public Component
{
    MaterialConstantBuffer _materialConstantBuffer;
    bool _isDirty = true;
public:
    MaterialComponent();
	MaterialComponent(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, float shininess);

    void SetAmbient(DirectX::XMFLOAT4 ambient);
    void SetDiffuse(DirectX::XMFLOAT4 diffuse);
    void SetSpecular(DirectX::XMFLOAT4 specular);
    void SetShininess(float shininess);

    DirectX::XMFLOAT4 GetAmbient() const;
    DirectX::XMFLOAT4 GetDiffuse() const;
    DirectX::XMFLOAT4 GetSpecular() const;
    float GetShininess() const;

    DirectX::XMFLOAT4& GetAmbientByRef();
    DirectX::XMFLOAT4& GetDiffuseByRef();
    DirectX::XMFLOAT4& GetSpecularByRef();
    float& GetShininessByRef();

    bool IsDirty() const;
    void SetIsDirty(bool isDirty);

    MaterialConstantBuffer GetMaterialConstantBuffer() const;

    static MaterialComponent GetDefaultMaterialComponent();

    template<typename Archive>
    void serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(_isDirty),
            cereal::make_nvp("ambient", _materialConstantBuffer.ambient),
            cereal::make_nvp("diffuse", _materialConstantBuffer.diffuse),
            cereal::make_nvp("specular", _materialConstantBuffer.specular),
            cereal::make_nvp("shininess", _materialConstantBuffer.shininess)
        );
    }
};

class MaterialComponentUI : public ComponentUI
{
public:
    void RenderUI(Component& component) override;
};