#include "RenderingApplication3D.hpp"
#include <string>
#include <algorithm>

int main(int argc, char* argv[])
{
    RenderingApplication3D application{ "D3D11 Engine" };
    application.Run();
}