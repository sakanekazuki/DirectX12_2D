/**
* @file Application.cpp
*/

#include "Application.h"

#include <iostream>

#include "Utility/Utility.h"
#include "Window/Window.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

namespace
{
// アプリケーションのインスタンス
Application* application = nullptr;
}

/**
* 初期化
*/
bool Application::Initialize()
{
	if (!application)
	{
		// アプリケーション作成
		application = new Application;
		if (!application)
		{
			Utility::DebugOutputFormatString("アプリケーション生成に失敗\n");

			return false;
		}

		Utility::DebugOutputFormatString("アプリケーション生成\n");

		// ウィンドウ作成
		WindowManagement::CraeteWindow(application->hwnd,
			application->windowClass,
			application->windowWidth,
			application->windowHeight,
			application->windowName);

		// デバイス初期化
		if (FAILED(application->CreateDevice()))
		{
			assert(0);
			return false;
		}

		if (FAILED(application->CreateCommand()))
		{
			assert(0);
			return false;
		}
	}
	else
	{
		Utility::DebugOutputFormatString("アプリケーションは生成済\n");
	}

	return true;
}

/**
* インスタンス取得
*/
Application& Application::Instance()
{
	return *application;
}

/**
* 後処理
*/
void Application::Finalize()
{
	if (application)
	{
		// 登録解除
		UnregisterClass(application->windowClass.lpszClassName, application->windowClass.hInstance);

		// アプリケーション削除
		delete application;
		application = nullptr;
	}
}

/**
* アプリケーションのループ
*/
void Application::Run()
{
	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);
	MSG msg = {};
	// メインループ
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// 終了
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}
}

Application::Application()
	:windowName("test")
{

}

/**
* デバイス作成
*/
HRESULT Application::CreateDevice()
{
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(result))
	{
		return result;
	}
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;
	for (int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);
		std::wstring strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	result = S_FALSE;
	// Direct3Dデバイスの初期化
	D3D_FEATURE_LEVEL featureLevel;
	for (auto lv : levels)
	{
		if (SUCCEEDED(D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(device.ReleaseAndGetAddressOf()))))
		{
			featureLevel = lv;
			result = S_OK;
			break;
		}
	}

	return result;
}

/**
* コマンド作成
*/
HRESULT Application::CreateCommand()
{
	auto result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(commandAllocator.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator.Get(),
		nullptr, 
		IID_PPV_ARGS(&commandList));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	// タイムアウトなし
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// アダプタを1つしか使わない
	cmdQueueDesc.NodeMask = 0;
	// プライオリティは特に指定なし
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// コマンドリストと合わせる
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	// キュー生成
	result = device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(commandQueue.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		assert(0);
	}
	return S_OK;
}