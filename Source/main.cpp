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
	// 頂点座標
	DirectX::XMFLOAT3 pos;
	// 法線ベクトル
	DirectX::XMFLOAT3 normal;
	// uv座標
	DirectX::XMFLOAT2 uv;
	// ボーン番号
	unsigned short boneNo[2];
	// ボーン影響度
	unsigned char boneWeight;
	// 輪郭線フラグ
	unsigned char edgeFlg;
};

/**
* @brief コンソール画面にフォーマット付き文字列を表示
* @param format フォーマット
* @param 可変長引数
* @remarks この関数はデバッグ用です。デバッグ時にしか動作しません
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
	// ウィンドウが破棄されたら呼ばれる
	if (msg == WM_DESTROY)
	{
		// アプリ終了通知
		PostQuitMessage(0);
		return 0;
	}
	// 既定の処理を行う
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
	
	// ウィンドウ作成・登録
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	// コールバック関数指定
	w.lpfnWndProc = (WNDPROC)WindowProcedure;
	// アプリケーションクラス名
	w.lpszClassName = _T("DX12Sample");
	// ハンドルの取得
	w.hInstance = GetModuleHandle(nullptr);

	// アプリケーションクラス
	RegisterClassEx(&w);

	// ウィンドウサイズを決める
	RECT wrc = { 0,0,window_width,window_height };
	// ウィンドウのサイズ補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウオブジェクトの作成
	HWND hwnd = CreateWindow(
		w.lpszClassName,      // クラス名指定
		_T("DX12テスト"),     // タイトルバーの文字設定
		WS_OVERLAPPEDWINDOW,  // タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,        // 表示x座標はOSに任せる
		CW_USEDEFAULT,        // 表示y座標はOSに任せる
		wrc.right - wrc.left, // ウィンドウの幅
		wrc.bottom - wrc.top, // ウィンドウの高さ
		nullptr,              // 親ウィンドウハンドル
		nullptr,              // メニューハンドル
		w.hInstance,          // 呼びだりアプリケーションハンドル
		nullptr);             // 追加パラメータ

#ifdef _DEBUG
	// デバッグレイヤーを有効化
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

	// アダプタ列挙用
	std::vector<IDXGIAdapter*> adapters;

	// 特定の名前を持つアダプタオブジェクトが入る
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
		// アダプタの説明オブジェクト取得
		adpt->GetDesc(&adesc);

		std::wstring strDesc = adesc.Description;

		// 探したいアダプタの名前を確認
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	// Direct3Dデバイス初期化
	D3D_FEATURE_LEVEL featureLevel;

	for (auto lv : levels)
	{
		if (D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&dev)) == S_OK)
		{
			featureLevel = lv;
			break;
		}
	}

	// コマンドアロケーター作成
	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
	// コマンドリスト作成
	result = dev->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator,
		nullptr,
		IID_PPV_ARGS(&cmdList));

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	// タイムアウトなし
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// アダプタを1つしか使わないときは0でよい
	cmdQueueDesc.NodeMask = 0;
	// プライオリティは特に指定なし
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// コマンドリストと合わせる
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	// コマンドキュー作成
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

	// バックバッファは伸び縮み可能
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	// フリップ後は速やかに破棄
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// 特に指定なし
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// ウィンドウとフルスクリーン切り替え可能
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// スワップチェーン作成
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue,
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&swapChain);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// レンダーターゲットビューなのでRTV
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	// 表と裏の2つ
	heapDesc.NumDescriptors = 2;
	// 特に指定なし
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// デスクリプタヒープ
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

	// フェンス
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	result = dev->CreateFence(
		fenceVal,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence));

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

	struct PMDHeader
	{
		float version;
		// モデル名
		char model_name[20];
		// モデルコメント
		char momment[256];
	};

	char signature[3] = {};
	PMDHeader pmdheader = {};
	auto fp = fopen("Resource/Model/初音ミク.pmd", "rb");

	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	// 頂点数
	unsigned int vertNum;
	fread(&vertNum, sizeof(vertNum), 1, fp);

	// 頂点1つあたりのサイズ
	constexpr size_t pmdvertex_size = 38;

	// バッファ確保
	std::vector<unsigned char> vertices(vertNum * pmdvertex_size);
	fread(vertices.data(), vertices.size(), 1, fp);

	fclose(fp);

	//struct Vertex
	//{
	//	// 座標
	//	DirectX::XMFLOAT3 pos;
	//	// uv座標
	//	DirectX::XMFLOAT2 uv;
	//};

	//// 3頂点
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
	// 頂点情報が入るだけのサイズ
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
	// バッファの仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 全バイト数
	vbView.SizeInBytes = vertices.size();
	// 1頂点あたりのバイト数
	vbView.StrideInBytes = pmdvertex_size;

	unsigned short indices[] =
	{
		0,1,2,
		2,1,3
	};

	ID3D12Resource* idxBuff = nullptr;

	// 設定は、バッファのサイズ以外、頂点バッファの設定を使いまわしてよい
	resdesc.Width = sizeof(indices);

	result = dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&idxBuff));

	// 作ったバッファにインデックスデータをコピー
	unsigned short* mappedIdx = nullptr;
	idxBuff->Map(0, nullptr, (void**)&mappedIdx);

	std::copy(std::begin(indices), std::end(indices), mappedIdx);
	idxBuff->Unmap(0, nullptr);

	// インデックスバッファビューを作成
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
			::OutputDebugStringA("ファイルが見つかりません");
		}
		else
		{
			// 受け取り用string
			std::string errstr;
			// 必要なサイズを確保
			errstr.resize(errorBlob->GetBufferSize());

			// データコピー
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";

			// データ表示
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
			::OutputDebugStringA("ファイルが見つかりません");
		}
		else
		{
			// 受け取り用string
			std::string errstr;
			// 必要なサイズを確保
			errstr.resize(errorBlob->GetBufferSize());
			// データコピー
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// データ表示
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
	// デフォルトのサンプルマスクを表す定数
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	// まだ安置エイリアスは使わないためfalse
	gpipeline.RasterizerState.MultisampleEnable = false;
	// カリングしない
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// 中身を塗りつぶす
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	// 震度方向のクリッピングは有効
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

	// レイアウト戦闘アドレス
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	// レイアウト配列の要素数
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// カットなし
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	// 三角形で構成
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// 今は1つのみ
	gpipeline.NumRenderTargets = 1;
	// 0～1に正規化されたRGBA
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// サンプリングは1ピクセルにつき1
	gpipeline.SampleDesc.Count = 1;
	// クオリティは最低
	gpipeline.SampleDesc.Quality = 0;

	ID3D12RootSignature* rootsignature = nullptr;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};

	// テクスチャ1つ
	descTblRange[0].NumDescriptors = 1;
	// 種別はテクスチャ
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// 0番スロットから
	descTblRange[0].BaseShaderRegister = 0;
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// 定数1つ
	descTblRange[1].NumDescriptors = 1;
	// 種別は定数
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 0番スロットから
	descTblRange[1].BaseShaderRegister = 0;
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootparam = {};
	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// 配列先頭アドレス
	rootparam.DescriptorTable.pDescriptorRanges = descTblRange;
	// ディスクリプタレンジ数
	rootparam.DescriptorTable.NumDescriptorRanges = 2;
	// 全てのシェーダーから見える
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//D3D12_ROOT_PARAMETER rootparam[2] = {};
	//rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//// デスクリプタレンジのアドレス
	//rootparam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
	//// ディスクリプタレンジ数
	//rootparam[0].DescriptorTable.NumDescriptorRanges = 1;
	//// ピクセルシェーダーから見える
	//rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootparam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];
	//rootparam[1].DescriptorTable.NumDescriptorRanges = 1;
	//rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	// ルートパラメーターの先頭アドレス
	rootSignatureDesc.pParameters = &rootparam;
	// ルートパラメーター数
	rootSignatureDesc.NumParameters = 1;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	// 横方向の繰り返し
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// 縦方向の繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// 奥行方向の繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// ボーダーは黒
	samplerDesc.BorderColor =
		D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	// 線形補間
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	// ミップマップ最大値
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	// ミップマップ最小値
	samplerDesc.MinLOD = 0.0f;
	// ピクセルシェーダーから見える
	samplerDesc.ShaderVisibility =
		D3D12_SHADER_VISIBILITY_PIXEL;
	// リサンプリングしない
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
	// 解放
	rootSigBlob->Release();

	gpipeline.pRootSignature = rootsignature;

	// ルートシグネチャ作成
	ID3D12PipelineState* pipelinestate = nullptr;
	result = dev->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&pipelinestate));

	D3D12_VIEWPORT viewport = {};

	// 出力先の幅
	viewport.Width = window_width;
	// 出力先の高さ
	viewport.Height = window_height;
	// 出力先の左上座標X
	viewport.TopLeftX = 0;
	// 出力先の左上座標Y
	viewport.TopLeftY = 0;
	// 震度最大値
	viewport.MaxDepth = 1.0f;
	// 震度最小値
	viewport.MinDepth = 0.0f;

	D3D12_RECT scissorrect = {};
	// 切り抜き上座標
	scissorrect.top = 0;
	// 切り抜き左座標
	scissorrect.left = 0;
	// 切り抜き右座標
	scissorrect.right = scissorrect.left + window_width;
	// 切り抜き左下座標
	scissorrect.bottom = scissorrect.top + window_height;

	// Wicテクスチャのロード
	DirectX::TexMetadata metadata = {};
	DirectX::ScratchImage scratchImg = {};
	result = DirectX::LoadFromWICFile(L"Resource/Texture/textest.png", DirectX::WIC_FLAGS_NONE, &metadata, scratchImg);
	// 生データ抽出
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

	// WriteToSubresourceで転送するためのヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	// 特殊な設定なのでDEFAULTでもUPLOADでもない
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// ライトバック
	texHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// 転送はL0、つまりCPU側から直接行う
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// 単一アダプタのための0
	texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	resDesc.Format = metadata.format;
	// 幅
	resDesc.Width = metadata.width;
	// 高さ
	resDesc.Height = metadata.height;
	// 2Dで配列でもないので1
	resDesc.DepthOrArraySize = metadata.arraySize;
	// 通常テクスチャなのでアンチエイリアシングしない
	resDesc.SampleDesc.Count = 1;
	// クオリティは最低
	resDesc.SampleDesc.Quality = 0;
	// ミップマップしないでミップ数は1つ
	resDesc.MipLevels = metadata.mipLevels;
	// 2Dテクスチャ用
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	// レイアウトは決定しない
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// 特にフラグなし
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* texbuff = nullptr;

	result = dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE, // 特になし
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texbuff));

	result = texbuff->WriteToSubresource(
		0,
		nullptr, // 全領域へコピー 
		img->pixels, // 元データアドレス
		img->rowPitch, // 1ラインサイズ
		img->slicePitch); // 全サイズ

	DirectX::XMMATRIX worldMat = DirectX::XMMatrixRotationY(DirectX::XM_PIDIV4);

	DirectX::XMFLOAT3 eye(0, 10, -15);
	DirectX::XMFLOAT3 target(0, 10, 0);
	DirectX::XMFLOAT3 up(0, 1, 0);

	auto viewMat = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&eye),
		DirectX::XMLoadFloat3(&target),
		DirectX::XMLoadFloat3(&up));

	auto projMap = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XM_PIDIV2, // 画角は90度
		static_cast<float>(window_width) / static_cast<float>(window_height), // アスペクト比
		1.0f, // 近いほう
		10.0f // 遠いほう
	);
	//matrix.r[0].m128_f32[0] = 2.0f / window_width;
	//matrix.r[1].m128_f32[1] = -2.0f / window_height;
	//matrix.r[3].m128_f32[0] = -1.0f;
	//matrix.r[3].m128_f32[1] = 1.0f;
	// 定数バッファ作成
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

	// マップ先を示すポインタ
	DirectX::XMMATRIX* mapMatrix;
	// マップ
	result = constBuff->Map(0, nullptr, (void**)&mapMatrix);
	// 行列の内容をコピー
	*mapMatrix = worldMat;

	ID3D12DescriptorHeap* basicDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	// シェーダーから見えるように
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// マスクは0
	descHeapDesc.NodeMask = 0;
	// ビューは今のところ1つだけ
	descHeapDesc.NumDescriptors = 2;
	// シェーダーリソースビュー用
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// 生成
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// 2Dテクスチャ
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	// ミップマップは使用しないので1
	srvDesc.Texture2D.MipLevels = 1;

	// ディスクリプタ先頭ハンドルを取得しておく
	auto basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	// シェーダーリソースビューの作成
	dev->CreateShaderResourceView(
		texbuff, // ビューと関連付けるバッファ
		&srvDesc, // 先程設定したテクスチャ設定情報
		basicDescHeap->GetCPUDescriptorHandleForHeapStart() // ヒープのどこに割り当てるか
	);

	// 次に移動
	basicHeapHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = constBuff->GetDesc().Width;

	// 定数バッファビュー作成
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
		// アプリケーション終了時にWM_QUITになる
		if (msg.message == WM_QUIT)
		{
			break;
		}

		angle += 0.005f;
		worldMat = DirectX::XMMatrixRotationY(angle);
		*mapMatrix = viewMat * projMap;

		// バックバッファ取得
		auto bbIdx = swapChain->GetCurrentBackBufferIndex();

		//D3D12_RESOURCE_BARRIER barrierDesc = {};
		//// 遷移
		//barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		//// 特に指定なし
		//barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		//barrierDesc.Transition.pResource = backBuffers[bbIdx];
		//// バックバッファリソース
		//barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		//// 直前はPRESENT状態
		//barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		//// 今からレンダーターゲット状態
		//barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// バリア指定実行
		//cmdList->ResourceBarrier(1, &barrierDesc);

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		cmdList->ResourceBarrier(1, &barrier);

		cmdList->SetPipelineState(pipelinestate);

		// レンダーターゲット指定
		auto rtvH = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += static_cast<ULONG_PTR>(bbIdx * dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);

		// 黄色
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

		// 命令を閉じる
		cmdList->Close();

		// コマンドリスト実行
		ID3D12CommandList* cmdlists[] = { cmdList };
		cmdQueue->ExecuteCommandLists(1, cmdlists);

		cmdQueue->Signal(fence, ++fenceVal);

		if (fence->GetCompletedValue() != fenceVal)
		{
			// イベントハンドルの取得
			auto event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);

			// イベントが発生するまで待ち続ける
			WaitForSingleObject(event, INFINITE);

			// イベントハンドルを閉じる
			CloseHandle(event);
		}

		// アロケーターリセット
		cmdAllocator->Reset();
		// 再びコマンドリストをためる準備
		cmdList->Reset(cmdAllocator, pipelinestate);

		// フリップ
		swapChain->Present(1, 0);
	}

	// クラスの登録解除
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}