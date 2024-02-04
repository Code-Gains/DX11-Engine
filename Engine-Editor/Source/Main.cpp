#include "RenderingApplication3D.hpp"
#include "Universe.hpp"
#include <memory>
#include <string>
#include <algorithm>

int main(int argc, char* argv[])
{
    RenderingApplication3D application("D3D11 Engine");
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