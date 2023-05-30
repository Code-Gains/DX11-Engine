//#include "EngineApplication.hpp"
//#include "HelloTriangleApplication.hpp"
//#include "TexturingApplication.hpp"
#include "Setting3DApplication.hpp"

int main(int argc, char* argv[])
{
    Setting3DApplication application{ "D3D11 Engine" };
    application.Run();
}