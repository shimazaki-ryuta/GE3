#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <stdint.h>
#include <array>

//SRVディスクリプタ割り当てクラス

class SRVManager
{
public:
	//インスタンス取得
	static SRVManager* GetInstance() {
		static SRVManager instance;
		return &instance;
	}
	//初期化
	void Initialize(ID3D12Device* device,ID3D12DescriptorHeap* descriptorHeap);
	//srvディスクリプタを一個作る
	int32_t CreateSRV(ID3D12Resource* pResource,const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc);

	//エンジンで使用するハンドルからDirectX利用用ハンドルを返す
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t handle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(size_t handle);

	static const size_t kNumSrvDescriptors = 1024;
	ID3D12DescriptorHeap* srvDescriptorHeap_=nullptr;
	ID3D12Device* device_ = nullptr;
	uint32_t descriptorSize_;
	std::array<bool, kNumSrvDescriptors> isUses;
private:
	SRVManager() {};
	~SRVManager() {}; 
	SRVManager(const SRVManager&) = delete;
	SRVManager operator=(const SRVManager&) = delete;
};