#include "SRVManager.h"
#include "GetDescriptorHandle.h"
#include <cassert>
void SRVManager::Initialize(ID3D12Device* device,ID3D12DescriptorHeap* descriptorHeap) {
	device_ = device;
	srvDescriptorHeap_ = descriptorHeap;
	descriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	isUses[0] = true;
	//先頭はimguiでよやくされるため2個めから
	for (size_t i = 1; i < size_t(kNumSrvDescriptors);i++) {
		isUses[i] = false;
	}
}

int32_t SRVManager::CreateSRV(ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc) {
	uint32_t handle = 0;
	for (handle = 1; handle < kNumSrvDescriptors; handle++) {
		if (isUses[handle] == false) {
			isUses[handle] = true;
			break;
		}
	}
	if (handle >= size_t(kNumSrvDescriptors)) {
		//rangeOver
		assert(false);
		return -1;
	}
	device_->CreateShaderResourceView(pResource, pDesc, GetCPUHandle(handle));
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPUHandle(uint32_t handle) {
	return GetCPUDescriptorHandle(srvDescriptorHeap_,descriptorSize_,handle);
}
D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPUHandle(size_t handle) {
	return GetGPUDescriptorHandle(srvDescriptorHeap_, descriptorSize_, uint32_t(handle));
}