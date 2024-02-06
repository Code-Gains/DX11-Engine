#include "Editor.hpp"

Editor::Editor()
{
}

void Editor::Run()
{
    RenderingApplication3D application("D3D11 Engine");
    application.Initialize();

    std::unique_ptr<IEngineModule> engineModule;

    auto device = application.GetApplicationDevice();
    auto deviceContext = application.GetApplicationDeviceContext();

    engineModule = std::make_unique<Universe>(
        application.GetApplicationWindow(),
        device,
        deviceContext,
        static_cast<int>(application.GetWindowWidth()),
        static_cast<int>(application.GetWindowHeight())
    );

    application.AddEngineModule(std::move(engineModule));

    application.Run();
}
