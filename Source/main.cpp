/**
* @file main.cpp
*/

#include "Application.h"

#ifdef _DEBUG
int main()
#else
#include <Windows.h>
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
#endif
{
	Application::Initialize();

	Application& app = Application::Instance();

	app.Run();

	Application::Finalize();

	return 0;
}