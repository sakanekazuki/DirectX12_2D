/**
* @file Application.h
*/

#ifndef APPLICATION_H_INCLUDE
#define APPLICATION_H_INCLUDE

#include <Windows.h>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <vector>
#include <map>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <d3dx12.h>
#include <wrl.h>

// アプリケーションクラス
class Application
{
public:
	static bool Initialize();

	Application& Instance();
	void Run();

	void Terminate();

private:
	const unsigned int windowWidth = 1280;
	const unsigned int windowHeight = 720;

	WNDCLASSEX windowClass;
	HWND hwnd;

};

#endif // APPLICATION_H_INCLUDE