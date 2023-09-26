#include "World.hpp"

//Entity World::CreateEntity(const std::vector<int>& componentSet)
//{
//	int entityId = _nextId;
//	auto entity = Entity(entityId);
//	_entities[entityId] = ComponentStorageShelf(ComponentSet(componentSet));
//
//	_nextId++;
//	return entity;
//}
//
//int World::DeleteEntity(int id)
//{
//	_entities.erase(id);
//	return id;
//}
//
//bool World::HasEntityComponent(int entityId, int componentId) const
//{
//	auto entityIterator = _entities.find(entityId);
//
//	if (entityIterator == _entities.end())
//		return false;
//
//	int entityComponentSetId = entityIterator->first;
//	return _uniqueComponentSets.count(entityComponentSetId) != 0;
//}
//
//void World::UpdateEntities(float deltaTime)
//{
//}
//
//size_t World::GetEntityCount() const
//{
//	return _entities.size();
//}
//
//ComponentStorage::ComponentStorage()
//{
//}
//
//ComponentStorage::ComponentStorage(size_t componentSize, size_t componentCount) : _componentSize(componentSize), _componentCount(componentCount)
//{
//}
//
//std::shared_ptr<void> ComponentStorage::GetElements() const
//{
//	return _elements;
//}
//
//size_t ComponentStorage::GetComponentSize() const
//{
//	return _componentSize;
//}
//
//size_t ComponentStorage::GetComponentCount() const
//{
//	return _componentCount;
//}

World::World()
{
}

bool World::Initialize(HWND win32Window, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    _win32Window = win32Window;
	_lightConstantBufferData.Position = { -50.0f, 150.0f, 150.0f, 0.0f };
	_lightConstantBufferData.Ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
	_lightConstantBufferData.Diffuse = { 0.5f, 0.5f, 0.5f, 1.0f };
	_lightConstantBufferData.Specular = { 0.0f, 0.0f, 0.0f, 1.0f };

	_instanceRenderer = InstanceRenderer(device, deviceContext);
	return true;
}

void World::Update(float deltaTime)
{
    // DirectX namespace contains overloads for vector and float multiplication
    using namespace DirectX;

    float cameraMoveSpeed = 10.0f;
    float cameraRotationSpeed = 5.0f;

    static DirectX::XMFLOAT3 cameraPosition = { 0.0f, 0.0f, 10.0f };
    static DirectX::XMFLOAT3 cameraRotation = { 0.0f,  Constants::DegreesToRadians(180), 0.0f };


    // Camera Movement

    if (GetAsyncKeyState('W') & 0x8000)
    {
        // Move camera forward
        DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
        DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
        DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + forward * cameraMoveSpeed * deltaTime;
        XMStoreFloat3(&cameraPosition, newPosition);
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        // Move camera backward
        DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
        DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), rotationMatrix);
        DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + forward * cameraMoveSpeed * deltaTime;
        XMStoreFloat3(&cameraPosition, newPosition);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        // Move camera left
        DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
        DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
        DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + right * cameraMoveSpeed * deltaTime;
        XMStoreFloat3(&cameraPosition, newPosition);
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        // Move camera right
        DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
        DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
        DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) - right * cameraMoveSpeed * deltaTime;
        XMStoreFloat3(&cameraPosition, newPosition);
    }


    static float lastMouseX = 0.0f;
    static float lastMouseY = 0.0f;

    bool isRightMouseDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    static bool wasRightMouseDown = false; // Keep track of previous right mouse button state
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(_win32Window, &cursorPos);
    int mouseX = cursorPos.x;
    int mouseY = cursorPos.y;

    if (isRightMouseDown) {
        if (!wasRightMouseDown) {
            // Right mouse button was just pressed, initialize previous mouse position
            lastMouseX = mouseX;
            lastMouseY = mouseY;
        }

        // Calculate the change in mouse position since the last frame
        int deltaX = mouseX - lastMouseX;
        int deltaY = mouseY - lastMouseY;

        // Update the camera rotation based on the change in mouse position
        cameraRotation.y -= deltaX * cameraRotationSpeed * deltaTime;
        cameraRotation.x += deltaY * cameraRotationSpeed * deltaTime;

        // Clamp pitch to prevent camera flipping
        cameraRotation.x = (-DirectX::XM_PIDIV2, min(DirectX::XM_PIDIV2, cameraRotation.x));

        // Update the previous mouse position
        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }

    wasRightMouseDown = isRightMouseDown;

    DirectX::XMVECTOR camPos = XMLoadFloat3(&cameraPosition);

    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);

    // Calculate the forward, right, and up vectors
    DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
    DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
    DirectX::XMVECTOR up = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);

    // Calculate the new camera target
    DirectX::XMVECTOR cameraTarget = DirectX::XMVectorAdd(XMLoadFloat3(&cameraPosition), forward);

    // Create the view matrix
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtRH(XMLoadFloat3(&cameraPosition), cameraTarget, up);

    DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovRH(Constants::DegreesToRadians(90),
        static_cast<float>(_viewportWidth) / static_cast<float>(_viewportHeight),
        0.1f,
        400);
    DirectX::XMMATRIX viewProjection = DirectX::XMMatrixMultiply(view, proj);
    DirectX::XMStoreFloat4x4(&_perFrameConstantBufferData.viewProjectionMatrix, viewProjection);

    _cameraConstantBufferData.cameraPosition = cameraPosition;

    //std::cout << _viewportWidth << " " << _viewportHeight << " " << &_win32Window << std::endl;

	_renderingSystem.Update(deltaTime);
}

void World::PeriodicUpdate(float deltaTime)
{
}

void World::Render()
{
	//_renderingSystem.Render(deviceContext, perObjectConstantBuffer, instanceConstantBuffer,
		//_transformComponents, _meshComponents, _materialComponents, _lightComponents);
	_instanceRenderer.RenderInstances<VertexPositionNormalUv>(_perFrameConstantBufferData, _cameraConstantBufferData, _lightConstantBufferData, MaterialConstantBuffer());
}

bool World::LoadWorld(std::string fileName)
{
	if(fileName != "")
	{
	}

	// Create a few meshes, materials, a light source

	auto entity = Entity();

	auto transformComponent = TransformComponent();
	//transformComponent.AddEntity(entity);
	_transformComponents.push_back(transformComponent);

	auto cube = Cube();
	auto meshComponent = MeshComponent(cube.GetVertices(), cube.GetIndices());
	//meshComponent.AddEntity(entity);
	_meshComponents.push_back(meshComponent);

	DirectX::XMFLOAT4 ambient{ 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 diffuse{ 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 specular{ 1.0f, 1.0f, 1.0f, 1.0f };
	float shininess = 3;


	auto materialComponent = MaterialComponent(ambient, diffuse, specular, shininess);
	//materialComponent.AddEntity(entity);
	_materialComponents.push_back(materialComponent);



	// Set up instance renderer
	std::vector<VertexPositionNormalUv> vertices = cube.GetVertices();
	std::vector<UINT> indices = cube.GetIndices();

	_instanceRenderer.InitializeInstancePool(0, vertices, indices);
	_instanceRenderer.AddInstance(InstanceConstantBuffer(cube.transform.GetWorldMatrix()), 0);

	std::cout << entity.GetId() << std::endl;

	return false;
}

void World::UpdateViewportDimensions(int32_t width, int32_t height)
{
	_viewportWidth = width;
	_viewportHeight = height;
}
