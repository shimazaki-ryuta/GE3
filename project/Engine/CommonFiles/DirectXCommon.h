#pragma once
//directx12関係
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <wrl.h>
#include "WinApp.h"
#include "../FixFPS.h"

#include "../externals/DirectXTex/DirectXTex.h"
#include "../externals/DirectXTex/d3dx12.h"

//#include <list>

class DirectXCommon
{
public:
	~DirectXCommon();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Window* win);



	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();
	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	//int32_t LoadTexture(const std::string& filePath);

	//画像データをGPUに送信する
	void  UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	inline ID3D12Device* GetDevice() { return device_.Get(); };
	inline ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); };
	inline ID3D12CommandQueue* GetCommandQueue() { return commandQueue_.Get(); };
	inline ID3D12CommandAllocator* GetCommandAllocator() { return commandAllocator_.Get(); };
	inline void SetDescriptorSizeSRV(uint32_t descriptorSizeSRV) { descriptorSizeSRV_ = descriptorSizeSRV; };
	inline void SetDescriptorSizeRTV(uint32_t descriptorSizeRTV) { descriptorSizeRTV_ = descriptorSizeRTV; };
	inline void SetDescriptorSizeDSV(uint32_t descriptorSizeDSV) { descriptorSizeDSV_ = descriptorSizeDSV; };

	static ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	static ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	//仮
	inline ID3D12DescriptorHeap* GetsrvDescriptorHeap() { return srvDescriptorHeap_.Get(); };

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	//fps固定の有無
	void FixedFPS(bool is) { isFixed_ = is; };


	// デスクリプターの数
	static const size_t kNumSrvDescriptors = 512;

private:
	Window* win_=nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;

	//コマンド関係
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//swapChain
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr };
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	//discriptorHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;


	//RenderTarget
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	//DepthBuffer
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_  = nullptr;

	//Fence
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	HANDLE fenceEvent_;
	uint64_t fenceValue_ = 0;
	//TransitionBarrier
	D3D12_RESOURCE_BARRIER barrier_{};

	//PSO


	//texture
	std::list<DirectX::ScratchImage*> mipImages_;
	//std::list<const DirectX::TexMetadata&> metadatas_;
	std::list<Microsoft::WRL::ComPtr<ID3D12Resource>> textureResources_;
	int32_t textureCount_=0;


	uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	//Fps固定用
	FixFPS fixFps_;
	bool isFixed_;

private:
	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	void InitializeDXGIDevice();
	/// <summary>
	/// コマンド生成
	/// </summary>
	void CreateCommand();
	/// <summary>
	/// スワップチェーン生成
	/// </summary>
	void CreateSwapChain();
	/// <summary>
	/// RTV生成
	/// </summary>
	void CreateRenderTargetView();
	
	/// <summary>
	/// SRV生成
	/// </summary>
	void CreateShaderResourceView();

	/// <summary>
	/// DSV生成
	/// </summary>
	void CreateDepthStencilView();


	/// <summary>
	/// Fence生成
	/// </summary>
	void CreateFence();

	/// <summary>
	/// ImGui初期化
	/// </summary>
	void InitializeImGui();
};
