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

#include "StandardTemplateLibrary/String.h"

// アプリケーションクラス
class Application
{
public:
	static bool Initialize();
	static Application& Instance();
	static void Finalize();

	void Run();

private:
	Application();
	~Application() = default;
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	HRESULT CreateDevice();

	const unsigned int windowWidth = 1280;
	const unsigned int windowHeight = 720;

	STL::String windowName;

	WNDCLASSEX windowClass = {};
	HWND hwnd;

	// dxgi
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;

	// デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;

};

#endif // APPLICATION_H_INCLUDE