#include "Headers/RenderingApplication3D.hpp"
#include "Universe.hpp"
#include <memory>
#include <string>
#include <algorithm>

int main(int argc, char* argv[])
{

    std::vector<std::unique_ptr<IEngineModule>> engineModules;
    engineModules.push_back(std::make_unique<Universe>());

    RenderingApplication3D application("D3D11 Engine", engineModules);

    application.Run();
}