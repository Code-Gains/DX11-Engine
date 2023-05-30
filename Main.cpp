//#include "EngineApplication.hpp"
#include "HelloTriangleApplication.hpp"
#include "TexturingApplication.hpp"

int main(int argc, char* argv[])
{
    TexturingApplication application{ "D3D11 Engine" };
    application.Run();
}