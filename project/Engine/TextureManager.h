#pragma once
//メモリリークチェック
#include "D3DResourceLeakChacker.h"
#include <memory>
#include <d3d12.h>
#include "CommonFiles/DirectXCommon.h"
#include <array>
#include <string>
#include <wrl.h>

//テクスチャ管理クラス

class TextureManager
{
public:
	//static std::shared_ptr<D3DResourceLeakChacker> leakchecker;
	struct Texture
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		//D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		//D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
		uint32_t srvHandle;
		std::string name;
	};

	//インスタンス取得
	static TextureManager* GetInstance();
	//初期化
	void Initialize(ID3D12Device*, std::string directoryPath = "Resources/");
	
	//GetterSetter
	inline void SetCommandQueue(ID3D12CommandQueue* commandQueue) {  commandQueue_ = commandQueue; };
	inline void SetCommandAllocator(ID3D12CommandAllocator* commandAllocator) { commandAllocator_ = commandAllocator; };
	inline void SetCommandList(ID3D12GraphicsCommandList* commandList) { commandList_ = commandList; };

	inline void SetDirectXCommon(DirectXCommon* dxCommon) { dxCommon_ = dxCommon; };
	inline void SetsrvDescriptorHeap(ID3D12DescriptorHeap* descriptorHeap) { srvDescriptorHeap_ = descriptorHeap; };

	void SetGraphicsRootDescriptorTable(UINT rootParamIndex, uint32_t textureHandle);
	const D3D12_RESOURCE_DESC GetResoureDesc(uint32_t textureHandle);
	ID3D12Resource* GetResoure(uint32_t textureHandle);

	//テクスチャ読み込み
	static uint32_t LoadTexture(const std::string&);

	//テクスチャ読み込み(LoadTextureの内部)
	uint32_t Load(const std::string&);

	//SRVディスクリプタヒープのテクスチャ領域の終端
	static const size_t kSrvTextureUseEnd = 256;
	//SRVディスクリプタヒープのテクスチャ領域の終端+1
	//static const size_t kSrvTextureUseEnd = 257;

private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager operator=(const TextureManager&) = delete;

	ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metadata);

	ID3D12Device* device_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	//
	ID3D12CommandQueue* commandQueue_ = nullptr;
	ID3D12CommandAllocator* commandAllocator_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;


	UINT descriptorHandleSize;
	std::string directoryPath_;

	uint32_t indexNextUseDiscriptorHeap = 0u;
	ID3D12DescriptorHeap* srvDescriptorHeap_ = nullptr;
	std::array<Texture, kSrvTextureUseEnd> textures_;

};

