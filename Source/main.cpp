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


	return 0;
}