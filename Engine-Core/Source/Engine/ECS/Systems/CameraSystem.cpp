#include "CameraSystem.hpp"

CameraSystem::CameraSystem()
{
}

CameraSystem::CameraSystem(ECS* ecs) : _ecs(ecs)
{
}

CameraSystem::~CameraSystem()
{
}

void CameraSystem::Render()
{
}

void CameraSystem::Update(float deltaTime)
{
    //// querry returns a vector of tuples that contain component vectors
    //auto componentQueryResult = _ecs->QueryComponentVectors<CameraComponent>();
    //// iterate
    //for (auto& tuple : componentQueryResult)
    //{
    //    auto& cameras = std::get<0>(tuple);
    //    // We want to work with the std::vector
    //    auto& rawCameras = *cameras->GetRawVector();

    //    for (auto& entityToComponent : cameras->GetEntityToIndex())
    //    {
    //        auto id = entityToComponent.first;
    //        auto idx = entityToComponent.second;

    //        // Run Camera Input Update

    //        using namespace DirectX;

    //        auto camera = rawCameras[idx];
    //        auto cameraPosition = camera.GetPosition();
    //        auto cameraRotation = camera.GetRotation();
    //        auto cameraMoveSpeed = camera.GetMoveSpeed();
    //        auto cameraRotationSpeed = camera.GetRotationSpeed();

    //        // Camera Movement

    //        if (GetAsyncKeyState('W') & 0x8000)
    //        {
    //            // Move camera forward
    //            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
    //            DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
    //            DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + forward * cameraMoveSpeed * deltaTime;
    //            XMStoreFloat3(&cameraPosition, newPosition);
    //        }
    //        if (GetAsyncKeyState('S') & 0x8000)
    //        {
    //            // Move camera backward
    //            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
    //            DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), rotationMatrix);
    //            DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + forward * cameraMoveSpeed * deltaTime;
    //            XMStoreFloat3(&cameraPosition, newPosition);
    //        }
    //        if (GetAsyncKeyState('A') & 0x8000)
    //        {
    //            // Move camera left
    //            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
    //            DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
    //            DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) + right * cameraMoveSpeed * deltaTime;
    //            XMStoreFloat3(&cameraPosition, newPosition);
    //        }
    //        if (GetAsyncKeyState('D') & 0x8000)
    //        {
    //            // Move camera right
    //            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
    //            DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
    //            DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&cameraPosition) - right * cameraMoveSpeed * deltaTime;
    //            XMStoreFloat3(&cameraPosition, newPosition);
    //        }


    //        static float lastMouseX = 0.0f;
    //        static float lastMouseY = 0.0f;

    //        bool isRightMouseDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    //        static bool wasRightMouseDown = false; // Keep track of previous right mouse button state
    //        POINT cursorPos;
    //        GetCursorPos(&cursorPos);
    //        ScreenToClient(_win32Window, &cursorPos);
    //        int mouseX = cursorPos.x;
    //        int mouseY = cursorPos.y;

    //        if (isRightMouseDown) {
    //            if (!wasRightMouseDown) {
    //                // Right mouse button was just pressed, initialize previous mouse position
    //                lastMouseX = mouseX;
    //                lastMouseY = mouseY;
    //            }

    //            // Calculate the change in mouse position since the last frame
    //            int deltaX = mouseX - lastMouseX;
    //            int deltaY = mouseY - lastMouseY;

    //            // Update the camera rotation based on the change in mouse position
    //            cameraRotation.y -= deltaX * cameraRotationSpeed * deltaTime;
    //            cameraRotation.x += deltaY * cameraRotationSpeed * deltaTime;

    //            // Clamp pitch to prevent camera flipping
    //            cameraRotation.x = (-DirectX::XM_PIDIV2, min(DirectX::XM_PIDIV2, cameraRotation.x));

    //            // Update the previous mouse position
    //            lastMouseX = mouseX;
    //            lastMouseY = mouseY;
    //        }

    //        wasRightMouseDown = isRightMouseDown;

    //        DirectX::XMVECTOR camPos = XMLoadFloat3(&cameraPosition);

    //        DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);

    //        // Calculate the forward, right, and up vectors
    //        DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
    //        DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
    //        DirectX::XMVECTOR up = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);

    //        // Calculate the new camera target
    //        DirectX::XMVECTOR cameraTarget = DirectX::XMVectorAdd(XMLoadFloat3(&cameraPosition), forward);

    //        // Create the view matrix
    //        DirectX::XMMATRIX view = DirectX::XMMatrixLookAtRH(XMLoadFloat3(&cameraPosition), cameraTarget, up);

    //        DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovRH(
    //            Constants::DegreesToRadians(90),
    //            static_cast<float>(_viewportWidth) / static_cast<float>(_viewportHeight),
    //            0.1f,
    //            400
    //        );

    //        DirectX::XMMATRIX viewProjection = DirectX::XMMatrixMultiply(view, proj);
    //        _renderingApplication->SetPerFrameConstantBuffer(viewProjection);
    //        _renderingApplication->SetCameraConstantBuffer(cameraPosition);

    //    }
    //}
}

void CameraSystem::PeriodicUpdate(float deltaTime)
{
}
