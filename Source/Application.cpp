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
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	// �f�o�b�O���C���[�L����
	debugLayer->EnableDebugLayer();
	// �L���������炢��t�i�[�t�F�C�X�����
	debugLayer->Release();
}

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

#ifdef _DEBUG
		EnableDebugLayer();
#endif

		// �E�B���h�E�}�l�[�W���[������
		WindowManagement::WindowManager::Initialize();
		// �E�B���h�E�}�l�[�W���[�擾
		WindowManagement::WindowManager& windowManager = WindowManagement::WindowManager::Instance();
		// �E�B���h�E�쐬
		application->hwnd = windowManager.CraeteWindow(
			application->windowWidth,
			application->windowHeight,
			application->windowName);

		application->windowClass = windowManager.GetWindowClass(application->windowName);

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
		if (FAILED(application->CreateSwapChain()))
		{
			assert(0);
			return false;
		}
		if (FAILED(application->CreateRenderTarget()))
		{
			assert(0);
			return false;
		}

		if (FAILED(application->CreateVertexBuffer()))
		{
			assert(0);
			return false;
		}

		if (FAILED(application->device->CreateFence(application->fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(application->fence.ReleaseAndGetAddressOf()))))
		{
			assert(0);
			return false;
		}

		if (FAILED(application->CreateRootSignature()))
		{
			return false;
		}

		if (FAILED(application->CreateGraphicsPipeline()))
		{
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
	WindowManagement::WindowManager& windowManager = WindowManagement::WindowManager::Instance();
	windowManager.ShowWindowByName(windowName);

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
		auto bbIdx = swapChain->GetCurrentBackBufferIndex();
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList->ResourceBarrier(1, &barrier);

		commandList->SetPipelineState(pipelineState.Get());

		auto rtvH = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		commandList->OMSetRenderTargets(1, &rtvH, true, nullptr);

		float clearColor[] = { 1.0f, 1.0f, 0.0f ,1.0f };

		commandList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissorrect);
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);

		commandList->SetGraphicsRootSignature(rootSignature.Get());

		commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

		barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		commandList->ResourceBarrier(1, &barrier);

		commandList->Close();

		ID3D12CommandList* cmdlists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(1, cmdlists);

		commandQueue->Signal(fence.Get(), ++fenceValue);

		if (fence->GetCompletedValue() != fenceValue)
		{
			auto event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceValue, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		commandAllocator->Reset();
		commandList->Reset(commandAllocator.Get(), pipelineState.Get());

		swapChain->Present(1, 0);
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
	//auto result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf()));
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

/**
* �X���b�v�`�F�C���쐬
*/
HRESULT Application::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

	swapChainDesc.Width = windowWidth;
	swapChainDesc.Height = windowHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	return dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)swapChain.ReleaseAndGetAddressOf());
}

/**
* �����_�[�^�[�Q�b�g�쐬
*/
HRESULT Application::CreateRenderTarget()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	auto result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(rtvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapChain->GetDesc(&swcDesc);
	backBuffers.resize(swcDesc.BufferCount);

	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	for (size_t idx = 0; idx < swcDesc.BufferCount; ++idx)
	{
		result = swapChain->GetBuffer(idx, IID_PPV_ARGS(&backBuffers[idx]));
		assert(SUCCEEDED(result));
		device->CreateRenderTargetView(backBuffers[idx], nullptr, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	viewport.Width = windowWidth;
	viewport.Height = windowHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;

	scissorrect.top = 0;
	scissorrect.left = 0;
	scissorrect.right = scissorrect.left + windowWidth;
	scissorrect.bottom = scissorrect.top + windowHeight;

	return result;
}


HRESULT Application::CreateVertexBuffer()
{
	DirectX::XMFLOAT3 vertices[] =
	{
		{-0.4f,-0.7f,0.0f},
		{-0.4f, 0.7f,0.0f},
		{ 0.4f,-0.7f,0.0f},
		{ 0.4f, 0.7f,0.0f},
	};

	D3D12_HEAP_PROPERTIES heapProp = {};

	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeof(vertices);
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	auto result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertexBuffer.ReleaseAndGetAddressOf()));

	DirectX::XMFLOAT3* vertexMap = nullptr;
	result = vertexBuffer->Map(0, nullptr, (void**)&vertexMap);
	std::copy(std::begin(vertices), std::end(vertices), vertexMap);
	vertexBuffer->Unmap(0, nullptr);

	unsigned short indices[] = { 0,1,2, 2,1,3 };

	resDesc.Width = sizeof(indices);

	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(idxBuffer.ReleaseAndGetAddressOf()));

	unsigned short* mappedIdx = nullptr;
	idxBuffer->Map(0, nullptr, (void**)&mappedIdx);

	std::copy(std::begin(indices), std::end(indices), mappedIdx);
	idxBuffer->Unmap(0, nullptr);

	ibView.BufferLocation = idxBuffer->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);

	// �o�b�t�@�̉��z�A�h���X
	vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	// �S�o�C�g��
	vbView.SizeInBytes = sizeof(vertices);
	// 1���_������̃o�C�g��
	vbView.StrideInBytes = sizeof(vertices[0]);

	return result;
}

/**
* �p�C�v���C���쐬
*/
HRESULT Application::CreateGraphicsPipeline()
{
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	auto result = D3DCompileFromFile(
		L"Source/Shader/BasicVertexShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob,
		&errorBlob);

	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			OutputDebugStringA("�t�@�C������������܂���");
			return 0;
		}
		else
		{
			std::string errstr;
			// �K�v�ȃT�C�Y�m��
			errstr.resize(errorBlob->GetBufferSize());

			// �f�[�^�R�s�[
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";

			OutputDebugStringA(errstr.c_str());
		}
		return result;
	}

	result = D3DCompileFromFile(
		L"Source/Shader/BasicPixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		&errorBlob);

	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			OutputDebugStringA("�t�@�C������������܂���");
			return 0;
		}
		else
		{
			std::string errstr;
			// �K�v�ȃT�C�Y�m��
			errstr.resize(errorBlob->GetBufferSize());

			// �f�[�^�R�s�[
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";

			OutputDebugStringA(errstr.c_str());
		}
		return result;
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.pRootSignature = rootSignature.Get();
	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();

	// �f�t�H���g�̃T���v���}�X�N��\���萔
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// �܂����u�G�C���A�X�͎g��Ȃ�
	gpipeline.RasterizerState.MultisampleEnable = false;
	// �J�����O���Ȃ�
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// �h��Ԃ�
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	// �[�x�����̃N���b�s���O�͗L��
	gpipeline.RasterizerState.DepthClipEnable = true;

	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	// �擪�A�h���X
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	// ���C�A�E�g�z��̗v�f��
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �J�b�g�Ȃ�
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	// �O�p�`�ō\��
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	gpipeline.SampleDesc.Count = 1;
	gpipeline.SampleDesc.Quality = 0;

	result = device->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(pipelineState.ReleaseAndGetAddressOf()));

	if (FAILED(result))
	{
		assert(0);
	}
	return result;
}

/**
* ���[�g�V�O�l�`���쐬
*/
HRESULT Application::CreateRootSignature()
{
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	auto result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSignatureBlob,
		&errorBlob);

	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	result = device->CreateRootSignature(
		0,
		rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));

	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	return result;
}