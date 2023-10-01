#include "3DRenderingApplication.hpp"
#include <string>
#include <algorithm>

int main(int argc, char* argv[])
{
    Rendering3DApplication application{ "D3D11 Engine" };
    application.Run();
}