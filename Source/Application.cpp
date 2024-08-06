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
// �A�v���P�[�V�����̃C���X�^���X
Application* application = nullptr;
}

/**
* ������
*/
bool Application::Initialize()
{
	if (!application)
	{
		// �A�v���P�[�V�����쐬
		application = new Application;
		if (!application)
		{
			Utility::DebugOutputFormatString("�A�v���P�[�V���������Ɏ��s\n");

			return false;
		}

		Utility::DebugOutputFormatString("�A�v���P�[�V��������\n");

		// �E�B���h�E�쐬
		WindowManagement::CraeteWindow(application->hwnd,
			application->windowClass,
			application->windowWidth,
			application->windowHeight,
			application->windowName);

		// �f�o�C�X������
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
		Utility::DebugOutputFormatString("�A�v���P�[�V�����͐�����\n");
	}

	return true;
}

/**
* �C���X�^���X�擾
*/
Application& Application::Instance()
{
	return *application;
}

/**
* �㏈��
*/
void Application::Finalize()
{
	if (application)
	{
		// �o�^����
		UnregisterClass(application->windowClass.lpszClassName, application->windowClass.hInstance);

		// �A�v���P�[�V�����폜
		delete application;
		application = nullptr;
	}
}

/**
* �A�v���P�[�V�����̃��[�v
*/
void Application::Run()
{
	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);
	MSG msg = {};
	// ���C�����[�v
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// �I��
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
* �f�o�C�X�쐬
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
	// Direct3D�f�o�C�X�̏�����
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
* �R�}���h�쐬
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
	// �^�C���A�E�g�Ȃ�
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// �A�_�v�^��1�����g��Ȃ�
	cmdQueueDesc.NodeMask = 0;
	// �v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// �R�}���h���X�g�ƍ��킹��
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	// �L���[����
	result = device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(commandQueue.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		assert(0);
	}
	return S_OK;
}