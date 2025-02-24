#include "Editor.hpp"
#include "CerealRegistry.hpp"
void LogCerealRegistration();

int main(int argc, char* argv[])
{
	LogCerealRegistration();
	Editor editor;
	editor.Run();
}