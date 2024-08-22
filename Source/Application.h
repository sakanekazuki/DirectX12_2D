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
#include <string>
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
	static Application& Instance();
	static void Finalize();

	void Run();

	ID3D12Resource* LoadTextureFromFile(std::string& filePath);

private:
	Application();
	~Application() = default;
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	HRESULT CreateDevice();
	HRESULT CreateCommand();
	HRESULT CreateSwapChain();
	HRESULT CreateRenderTarget();
	HRESULT CreateVertexBuffer();
	HRESULT CreateGraphicsPipeline();
	HRESULT CreateRootSignature();

	const unsigned int windowWidth = 1280;
	const unsigned int windowHeight = 720;

	std::string windowName;

	WNDCLASSEX windowClass = {};
	HWND hwnd = {};

	// dxgi
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;

	// デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> idxBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	UINT64 fenceValue = 0;

	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	std::vector<ID3D12Resource*> backBuffers;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr;
	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissorrect;


	std::unordered_map<std::string, ID3D12Resource*> textureResources;

};

#endif // APPLICATION_H_INCLUDE