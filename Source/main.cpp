/**
* @file main.cpp
*/

#include "Application.h"

#include "Window/Window.h"
#include "Manager/FileManager.h"
#include "Manager/ShaderManager.h"

#ifdef _DEBUG
int main()
#else
#include <Windows.h>
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
#endif
{
	Application::Initialize();

	Application& app = Application::Instance();

	File::FileManager::Initialize();
	Shader::ShaderManager::Initialize();

	app.Run();

	Shader::ShaderManager::Finalize();
	File::FileManager::Finalize();

	Application::Finalize();

	return 0;
}