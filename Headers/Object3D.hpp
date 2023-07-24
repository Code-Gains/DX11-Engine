#pragma once
#include <d3d11_2.h>
#include <vector>
#include <memory>
#include "Transform.hpp"

enum class PropertyType
{
    Material
};

class Property {
public:
    virtual ~Property() {}
};

class Object3D
{
    std::vector<Property> _properties;
public:
    Transform transform;

    Object3D() {};

    virtual ~Object3D() {};

    virtual bool Initialize(ID3D11Device* device) = 0;

    virtual void Update(const float deltaTime) = 0;

    virtual void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer) = 0;

    virtual std::vector<Object3D*> GetAllObjects();

};

class CompositeObject3D: public Object3D
{
protected:
    std::vector<Object3D*> _children;

public:
    virtual std::vector<Object3D*> GetAllObjects();
};