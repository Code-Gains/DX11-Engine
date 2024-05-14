#include "Editor.hpp"

Editor::Editor()
{
}

void Editor::Run()
{
    RenderingApplication3D application("CG Engine");
    application.Initialize();

    std::unique_ptr<IEngineModule> engineModule;

    auto device = application.GetApplicationDevice();
    auto deviceContext = application.GetApplicationDeviceContext();

    if (!application.Load())
    {
        std::cerr << "Could not load Rendering Application!\n";
        return;
    }

    engineModule = std::make_unique<Universe>(
        application.GetApplicationWindow(),
        &application,
        device,
        deviceContext,
        static_cast<int>(application.GetWindowWidth()),
        static_cast<int>(application.GetWindowHeight())
    );

    application.AddEngineModule(std::move(engineModule));

    application.Run();
}
