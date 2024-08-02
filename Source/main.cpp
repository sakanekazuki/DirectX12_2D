/**
* @file main.cpp
*/

#include <Windows.h>

#include <vector>
#include <tchar.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

#include <DirectXTex.h>

#include <d3dx12.h>

#ifdef _DEBUG
#include <iostream>
#endif

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

struct PMDVertex
{
	// ���_���W
	DirectX::XMFLOAT3 pos;
	// �@���x�N�g��
	DirectX::XMFLOAT3 normal;
	// uv���W
	DirectX::XMFLOAT2 uv;
	// �{�[���ԍ�
	unsigned short boneNo[2];
	// �{�[���e���x
	unsigned char boneWeight;
	// �֊s���t���O
	unsigned char edgeFlg;
};

/**
* @brief �R���\�[����ʂɃt�H�[�}�b�g�t���������\��
* @param format �t�H�[�}�b�g
* @param �ϒ�����
* @remarks ���̊֐��̓f�o�b�O�p�ł��B�f�o�b�O���ɂ������삵�܂���
*/
void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	printf_s(format, valist);
	va_end(valist);
#endif
}

/*
*/
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// �E�B���h�E���j�����ꂽ��Ă΂��
	if (msg == WM_DESTROY)
	{
		// �A�v���I���ʒm
		PostQuitMessage(0);
		return 0;
	}
	// ����̏������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

const unsigned int window_width = 1280;
const unsigned int window_height = 720;

ID3D12Device* dev = nullptr;
IDXGIFactory6* dxgiFactory = nullptr;
IDXGISwapChain4* swapChain = nullptr;
ID3D12CommandAllocator* cmdAllocator = nullptr;
ID3D12GraphicsCommandList* cmdList = nullptr;
ID3D12CommandQueue* cmdQueue = nullptr;

void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
	debugLayer->EnableDebugLayer();
	debugLayer->Release();
}

#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
	DebugOutputFormatString("Show window test");
	
	// �E�B���h�E�쐬�E�o�^
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	// �R�[���o�b�N�֐��w��
	w.lpfnWndProc = (WNDPROC)WindowProcedure;
	// �A�v���P�[�V�����N���X��
	w.lpszClassName = _T("DX12Sample");
	// �n���h���̎擾
	w.hInstance = GetModuleHandle(nullptr);

	// �A�v���P�[�V�����N���X
	RegisterClassEx(&w);

	// �E�B���h�E�T�C�Y�����߂�
	RECT wrc = { 0,0,window_width,window_height };
	// �E�B���h�E�̃T�C�Y�␳
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// �E�B���h�E�I�u�W�F�N�g�̍쐬
	HWND hwnd = CreateWindow(
		w.lpszClassName,      // �N���X���w��
		_T("DX12�e�X�g"),     // �^�C�g���o�[�̕����ݒ�
		WS_OVERLAPPEDWINDOW,  // �^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,        // �\��x���W��OS�ɔC����
		CW_USEDEFAULT,        // �\��y���W��OS�ɔC����
		wrc.right - wrc.left, // �E�B���h�E�̕�
		wrc.bottom - wrc.top, // �E�B���h�E�̍���
		nullptr,              // �e�E�B���h�E�n���h��
		nullptr,              // ���j���[�n���h��
		w.hInstance,          // �Ăт���A�v���P�[�V�����n���h��
		nullptr);             // �ǉ��p�����[�^

#ifdef _DEBUG
	// �f�o�b�O���C���[��L����
	EnableDebugLayer();

#endif

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

//#ifdef _DEBUG
//	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgiFactory));
//#else
//	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
//#endif

	// �A�_�v�^�񋓗p
	std::vector<IDXGIAdapter*> adapters;

	// ����̖��O�����A�_�v�^�I�u�W�F�N�g������
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0;
		dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		// �A�_�v�^�̐����I�u�W�F�N�g�擾
		adpt->GetDesc(&adesc);

		std::wstring strDesc = adesc.Description;

		// �T�������A�_�v�^�̖��O���m�F
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	// Direct3D�f�o�C�X������
	D3D_FEATURE_LEVEL featureLevel;

	for (auto lv : levels)
	{
		if (D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&dev)) == S_OK)
		{
			featureLevel = lv;
			break;
		}
	}

	// �R�}���h�A���P�[�^�[�쐬
	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
	// �R�}���h���X�g�쐬
	result = dev->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator,
		nullptr,
		IID_PPV_ARGS(&cmdList));

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	// �^�C���A�E�g�Ȃ�
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// �A�_�v�^��1�����g��Ȃ��Ƃ���0�ł悢
	cmdQueueDesc.NodeMask = 0;
	// �v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// �R�}���h���X�g�ƍ��킹��
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	// �R�}���h�L���[�쐬
	result = dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;

	// �o�b�N�o�b�t�@�͐L�яk�݉\
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	// �t���b�v��͑��₩�ɔj��
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// ���Ɏw��Ȃ�
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// �E�B���h�E�ƃt���X�N���[���؂�ւ��\
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// �X���b�v�`�F�[���쐬
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue,
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&swapChain);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// �����_�[�^�[�Q�b�g�r���[�Ȃ̂�RTV
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	// �\�Ɨ���2��
	heapDesc.NumDescriptors = 2;
	// ���Ɏw��Ȃ�
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// �f�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* rtvHeap = nullptr;
	result = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap));

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapChain->GetDesc(&swcDesc);

	std::vector<ID3D12Resource*> backBuffers(swcDesc.BufferCount);
	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (size_t idx = 0; idx < swcDesc.BufferCount; ++idx)
	{
		result = swapChain->GetBuffer(static_cast<UINT>(idx), IID_PPV_ARGS(&backBuffers[idx]));
		dev->CreateRenderTargetView(backBuffers[idx], &rtvDesc, handle);
		handle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// �t�F���X
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	result = dev->CreateFence(
		fenceVal,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence));

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

	struct PMDHeader
	{
		float version;
		// ���f����
		char model_name[20];
		// ���f���R�����g
		char momment[256];
	};

	char signature[3] = {};
	PMDHeader pmdheader = {};
	auto fp = fopen("Resource/Model/�����~�N.pmd", "rb");

	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	// ���_��
	unsigned int vertNum;
	fread(&vertNum, sizeof(vertNum), 1, fp);

	// ���_1������̃T�C�Y
	constexpr size_t pmdvertex_size = 38;

	// �o�b�t�@�m��
	std::vector<unsigned char> vertices(vertNum * pmdvertex_size);
	fread(vertices.data(), vertices.size(), 1, fp);

	fclose(fp);

	//struct Vertex
	//{
	//	// ���W
	//	DirectX::XMFLOAT3 pos;
	//	// uv���W
	//	DirectX::XMFLOAT2 uv;
	//};

	//// 3���_
	//Vertex vertices[] =
	//{
	//	{{-1.0f,-1.0f,0.0f},{0.0f,1.0f}},
	//	{{-1.0f, 1.0f,0.0f},{0.0f,0.0f}},
	//	{{ 1.0f,-1.0f,0.0f},{1.0f,1.0f}},
	//	{{ 1.0f, 1.0f,0.0f},{1.0f,0.0f}},
	//};

	D3D12_HEAP_PROPERTIES heapprop = {};

	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resdesc = {};

	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// ���_��񂪓��邾���̃T�C�Y
	resdesc.Width = sizeof(vertices);
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.SampleDesc.Count = 1;
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* vertBuff = nullptr;

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size());
	result = dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	unsigned char* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	vertBuff->Unmap(0, nullptr);

	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// �o�b�t�@�̉��z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// �S�o�C�g��
	vbView.SizeInBytes = vertices.size();
	// 1���_������̃o�C�g��
	vbView.StrideInBytes = pmdvertex_size;

	unsigned short indices[] =
	{
		0,1,2,
		2,1,3
	};

	ID3D12Resource* idxBuff = nullptr;

	// �ݒ�́A�o�b�t�@�̃T�C�Y�ȊO�A���_�o�b�t�@�̐ݒ���g���܂킵�Ă悢
	resdesc.Width = sizeof(indices);

	result = dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&idxBuff));

	// ������o�b�t�@�ɃC���f�b�N�X�f�[�^���R�s�[
	unsigned short* mappedIdx = nullptr;
	idxBuff->Map(0, nullptr, (void**)&mappedIdx);

	std::copy(std::begin(indices), std::end(indices), mappedIdx);
	idxBuff->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�r���[���쐬
	D3D12_INDEX_BUFFER_VIEW ibView = {};

	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;

	ID3DBlob* errorBlob = nullptr;
	result = D3DCompileFromFile(
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
			::OutputDebugStringA("�t�@�C����������܂���");
		}
		else
		{
			// �󂯎��pstring
			std::string errstr;
			// �K�v�ȃT�C�Y���m��
			errstr.resize(errorBlob->GetBufferSize());

			// �f�[�^�R�s�[
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";

			// �f�[�^�\��
			::OutputDebugStringA(errstr.c_str());
		}
		exit(1);
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
			::OutputDebugStringA("�t�@�C����������܂���");
		}
		else
		{
			// �󂯎��pstring
			std::string errstr;
			// �K�v�ȃT�C�Y���m��
			errstr.resize(errorBlob->GetBufferSize());
			// �f�[�^�R�s�[
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// �f�[�^�\��
			::OutputDebugStringA(errstr.c_str());
		}
		exit(1);
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
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"BONE_NO",
			0,
			DXGI_FORMAT_R16G16_UINT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"WEIGHT",
			0,
			DXGI_FORMAT_R8_UINT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"EDGE_FLG",
			0,
			DXGI_FORMAT_R8_UINT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.pRootSignature = nullptr;
	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();
	// �f�t�H���g�̃T���v���}�X�N��\���萔
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	// �܂����u�G�C���A�X�͎g��Ȃ�����false
	gpipeline.RasterizerState.MultisampleEnable = false;
	// �J�����O���Ȃ�
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// ���g��h��Ԃ�
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	// �k�x�����̃N���b�s���O�͗L��
	gpipeline.RasterizerState.DepthClipEnable = true;

	gpipeline.RasterizerState.FrontCounterClockwise = false;
	gpipeline.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	gpipeline.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	gpipeline.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	gpipeline.RasterizerState.AntialiasedLineEnable = false;
	gpipeline.RasterizerState.ForcedSampleCount = 0;
	gpipeline.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	gpipeline.DepthStencilState.DepthEnable = false;
	gpipeline.DepthStencilState.StencilEnable = false;

	// ���C�A�E�g�퓬�A�h���X
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	// ���C�A�E�g�z��̗v�f��
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �J�b�g�Ȃ�
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	// �O�p�`�ō\��
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// ����1�̂�
	gpipeline.NumRenderTargets = 1;
	// 0�`1�ɐ��K�����ꂽRGBA
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// �T���v�����O��1�s�N�Z���ɂ�1
	gpipeline.SampleDesc.Count = 1;
	// �N�I���e�B�͍Œ�
	gpipeline.SampleDesc.Quality = 0;

	ID3D12RootSignature* rootsignature = nullptr;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};

	// �e�N�X�`��1��
	descTblRange[0].NumDescriptors = 1;
	// ��ʂ̓e�N�X�`��
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// 0�ԃX���b�g����
	descTblRange[0].BaseShaderRegister = 0;
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// �萔1��
	descTblRange[1].NumDescriptors = 1;
	// ��ʂ͒萔
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 0�ԃX���b�g����
	descTblRange[1].BaseShaderRegister = 0;
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootparam = {};
	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �z��擪�A�h���X
	rootparam.DescriptorTable.pDescriptorRanges = descTblRange;
	// �f�B�X�N���v�^�����W��
	rootparam.DescriptorTable.NumDescriptorRanges = 2;
	// �S�ẴV�F�[�_�[���猩����
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//D3D12_ROOT_PARAMETER rootparam[2] = {};
	//rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//// �f�X�N���v�^�����W�̃A�h���X
	//rootparam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
	//// �f�B�X�N���v�^�����W��
	//rootparam[0].DescriptorTable.NumDescriptorRanges = 1;
	//// �s�N�Z���V�F�[�_�[���猩����
	//rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootparam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];
	//rootparam[1].DescriptorTable.NumDescriptorRanges = 1;
	//rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	// ���[�g�p�����[�^�[�̐擪�A�h���X
	rootSignatureDesc.pParameters = &rootparam;
	// ���[�g�p�����[�^�[��
	rootSignatureDesc.NumParameters = 1;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	// �������̌J��Ԃ�
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// �c�����̌J��Ԃ�
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// ���s�����̌J��Ԃ�
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// �{�[�_�[�͍�
	samplerDesc.BorderColor =
		D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	// ���`���
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	// �~�b�v�}�b�v�ő�l
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	// �~�b�v�}�b�v�ŏ��l
	samplerDesc.MinLOD = 0.0f;
	// �s�N�Z���V�F�[�_�[���猩����
	samplerDesc.ShaderVisibility =
		D3D12_SHADER_VISIBILITY_PIXEL;
	// ���T���v�����O���Ȃ�
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);
	result = dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));
	// ���
	rootSigBlob->Release();

	gpipeline.pRootSignature = rootsignature;

	// ���[�g�V�O�l�`���쐬
	ID3D12PipelineState* pipelinestate = nullptr;
	result = dev->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&pipelinestate));

	D3D12_VIEWPORT viewport = {};

	// �o�͐�̕�
	viewport.Width = window_width;
	// �o�͐�̍���
	viewport.Height = window_height;
	// �o�͐�̍�����WX
	viewport.TopLeftX = 0;
	// �o�͐�̍�����WY
	viewport.TopLeftY = 0;
	// �k�x�ő�l
	viewport.MaxDepth = 1.0f;
	// �k�x�ŏ��l
	viewport.MinDepth = 0.0f;

	D3D12_RECT scissorrect = {};
	// �؂蔲������W
	scissorrect.top = 0;
	// �؂蔲�������W
	scissorrect.left = 0;
	// �؂蔲���E���W
	scissorrect.right = scissorrect.left + window_width;
	// �؂蔲���������W
	scissorrect.bottom = scissorrect.top + window_height;

	// Wic�e�N�X�`���̃��[�h
	DirectX::TexMetadata metadata = {};
	DirectX::ScratchImage scratchImg = {};
	result = DirectX::LoadFromWICFile(L"Resource/Texture/textest.png", DirectX::WIC_FLAGS_NONE, &metadata, scratchImg);
	// ���f�[�^���o
	auto img = scratchImg.GetImage(0, 0, 0);

	/*struct TexRGBA
	{
		unsigned char R, G, B, A;
	};

	std::vector<TexRGBA> texturedata(256 * 256);
	for (auto& rgba : texturedata)
	{
		rgba.R = rand() % 256;
		rgba.G = rand() % 256;
		rgba.B = rand() % 256;
		rgba.A =255;
	}*/

	// WriteToSubresource�œ]�����邽�߂̃q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	// ����Ȑݒ�Ȃ̂�DEFAULT�ł�UPLOAD�ł��Ȃ�
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// ���C�g�o�b�N
	texHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// �]����L0�A�܂�CPU�����璼�ڍs��
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// �P��A�_�v�^�̂��߂�0
	texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	resDesc.Format = metadata.format;
	// ��
	resDesc.Width = metadata.width;
	// ����
	resDesc.Height = metadata.height;
	// 2D�Ŕz��ł��Ȃ��̂�1
	resDesc.DepthOrArraySize = metadata.arraySize;
	// �ʏ�e�N�X�`���Ȃ̂ŃA���`�G�C���A�V���O���Ȃ�
	resDesc.SampleDesc.Count = 1;
	// �N�I���e�B�͍Œ�
	resDesc.SampleDesc.Quality = 0;
	// �~�b�v�}�b�v���Ȃ��Ń~�b�v����1��
	resDesc.MipLevels = metadata.mipLevels;
	// 2D�e�N�X�`���p
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	// ���C�A�E�g�͌��肵�Ȃ�
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// ���Ƀt���O�Ȃ�
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* texbuff = nullptr;

	result = dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE, // ���ɂȂ�
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // �e�N�X�`���p�w��
		nullptr,
		IID_PPV_ARGS(&texbuff));

	result = texbuff->WriteToSubresource(
		0,
		nullptr, // �S�̈�փR�s�[ 
		img->pixels, // ���f�[�^�A�h���X
		img->rowPitch, // 1���C���T�C�Y
		img->slicePitch); // �S�T�C�Y

	DirectX::XMMATRIX worldMat = DirectX::XMMatrixRotationY(DirectX::XM_PIDIV4);

	DirectX::XMFLOAT3 eye(0, 10, -15);
	DirectX::XMFLOAT3 target(0, 10, 0);
	DirectX::XMFLOAT3 up(0, 1, 0);

	auto viewMat = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&eye),
		DirectX::XMLoadFloat3(&target),
		DirectX::XMLoadFloat3(&up));

	auto projMap = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XM_PIDIV2, // ��p��90�x
		static_cast<float>(window_width) / static_cast<float>(window_height), // �A�X�y�N�g��
		1.0f, // �߂��ق�
		10.0f // �����ق�
	);
	//matrix.r[0].m128_f32[0] = 2.0f / window_width;
	//matrix.r[1].m128_f32[1] = -2.0f / window_height;
	//matrix.r[3].m128_f32[0] = -1.0f;
	//matrix.r[3].m128_f32[1] = 1.0f;
	// �萔�o�b�t�@�쐬
	ID3D12Resource* constBuff = nullptr;
	heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	resDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(worldMat) + 0xff) & ~0xff);
	result = dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);

	// �}�b�v��������|�C���^
	DirectX::XMMATRIX* mapMatrix;
	// �}�b�v
	result = constBuff->Map(0, nullptr, (void**)&mapMatrix);
	// �s��̓��e���R�s�[
	*mapMatrix = worldMat;

	ID3D12DescriptorHeap* basicDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	// �V�F�[�_�[���猩����悤��
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// �}�X�N��0
	descHeapDesc.NodeMask = 0;
	// �r���[�͍��̂Ƃ���1����
	descHeapDesc.NumDescriptors = 2;
	// �V�F�[�_�[���\�[�X�r���[�p
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// ����
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// 2D�e�N�X�`��
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	// �~�b�v�}�b�v�͎g�p���Ȃ��̂�1
	srvDesc.Texture2D.MipLevels = 1;

	// �f�B�X�N���v�^�擪�n���h�����擾���Ă���
	auto basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	// �V�F�[�_�[���\�[�X�r���[�̍쐬
	dev->CreateShaderResourceView(
		texbuff, // �r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc, // ����ݒ肵���e�N�X�`���ݒ���
		basicDescHeap->GetCPUDescriptorHandleForHeapStart() // �q�[�v�̂ǂ��Ɋ��蓖�Ă邩
	);

	// ���Ɉړ�
	basicHeapHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = constBuff->GetDesc().Width;

	// �萔�o�b�t�@�r���[�쐬
	dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	MSG msg = {};
	float angle = 0.0f;
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// �A�v���P�[�V�����I������WM_QUIT�ɂȂ�
		if (msg.message == WM_QUIT)
		{
			break;
		}

		angle += 0.005f;
		worldMat = DirectX::XMMatrixRotationY(angle);
		*mapMatrix = viewMat * projMap;

		// �o�b�N�o�b�t�@�擾
		auto bbIdx = swapChain->GetCurrentBackBufferIndex();

		//D3D12_RESOURCE_BARRIER barrierDesc = {};
		//// �J��
		//barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		//// ���Ɏw��Ȃ�
		//barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		//barrierDesc.Transition.pResource = backBuffers[bbIdx];
		//// �o�b�N�o�b�t�@���\�[�X
		//barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		//// ���O��PRESENT���
		//barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		//// �����烌���_�[�^�[�Q�b�g���
		//barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// �o���A�w����s
		//cmdList->ResourceBarrier(1, &barrierDesc);

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		cmdList->ResourceBarrier(1, &barrier);

		cmdList->SetPipelineState(pipelinestate);

		// �����_�[�^�[�Q�b�g�w��
		auto rtvH = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += static_cast<ULONG_PTR>(bbIdx * dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);

		// ���F
		float clearColor[] = { 1.0f,1.0f,1.0f,1.0f };
		cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		cmdList->RSSetViewports(1, &viewport);
		cmdList->RSSetScissorRects(1, &scissorrect);
		cmdList->SetGraphicsRootSignature(rootsignature);

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		cmdList->IASetVertexBuffers(0, 1, &vbView);
		cmdList->IASetIndexBuffer(&ibView);

		cmdList->SetGraphicsRootSignature(rootsignature);
		cmdList->SetDescriptorHeaps(1, &basicDescHeap);
		//auto heapHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		//heapHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		cmdList->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());

		cmdList->DrawInstanced(vertNum, 1, 0, 0);
		//cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

		//barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		//barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		cmdList->ResourceBarrier(1, &barrier);

		// ���߂����
		cmdList->Close();

		// �R�}���h���X�g���s
		ID3D12CommandList* cmdlists[] = { cmdList };
		cmdQueue->ExecuteCommandLists(1, cmdlists);

		cmdQueue->Signal(fence, ++fenceVal);

		if (fence->GetCompletedValue() != fenceVal)
		{
			// �C�x���g�n���h���̎擾
			auto event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);

			// �C�x���g����������܂ő҂�������
			WaitForSingleObject(event, INFINITE);

			// �C�x���g�n���h�������
			CloseHandle(event);
		}

		// �A���P�[�^�[���Z�b�g
		cmdAllocator->Reset();
		// �ĂуR�}���h���X�g�����߂鏀��
		cmdList->Reset(cmdAllocator, pipelinestate);

		// �t���b�v
		swapChain->Present(1, 0);
	}

	// �N���X�̓o�^����
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}