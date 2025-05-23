#include "DirectXCommon.h"
#include "ConvertString.h"
#include <format>
#include "../TextureManager.h"

#include "2D/PostEffect.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

#include "../externals/imgui/imgui_impl_dx12.h"
#include "../externals/imgui/imgui_impl_win32.h"

#include "SRVManager.h"
namespace MyEngine {
	ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
	{
		ID3D12DescriptorHeap* descreptorHeap = nullptr;
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
		descriptorHeapDesc.Type = heapType;
		descriptorHeapDesc.NumDescriptors = numDescriptors;
		descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descreptorHeap));
		assert(SUCCEEDED(hr));
		return descreptorHeap;
	}
	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height)
	{
		//生成するResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = width;
		resourceDesc.Height = height;
		resourceDesc.MipLevels = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		//利用するHeapの設定
		D3D12_HEAP_PROPERTIES heapPropaerties{};
		heapPropaerties.Type = D3D12_HEAP_TYPE_DEFAULT;

		//深度値のクリア設定
		D3D12_CLEAR_VALUE depthClearValue{};
		depthClearValue.DepthStencil.Depth = 1.0f;
		depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		//Resourceの生成
		ID3D12Resource* resource = nullptr;
		HRESULT hr = device->CreateCommittedResource(&heapPropaerties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthClearValue, IID_PPV_ARGS(&resource));
		assert(SUCCEEDED(hr));
		return resource;
	}

	DirectXCommon::~DirectXCommon()
	{
		/*
		//texture
		for (ID3D12Resource* textureResource : textureResources_)
		{
			textureResource->Release();
		}*/
		for (DirectX::ScratchImage* mipImage : mipImages_)
		{
			delete mipImage;
		}
		//textureResources_.clear();
		//mipImages_.clear();

		CloseHandle(fenceEvent_);
		//fence_->Release();
		//rtvDescriptorHeap_->Release();
		//srvDescriptorHeap_->Release();
		//dsvDescriptorHeap_->Release();
		//depthStencilResource_->Release();

		//swapChainResources_[0]->Release();
		//swapChainResources_[1]->Release();
		//swapChain_->Release();
		//commandList_->Release();
		//commandAllocator_->Release();
		//commandQueue_->Release();
		//device_->Release();
		//useAdapter_->Release();
		//dxgiFactory_->Release();
	}
	void DirectXCommon::DeletePostEffect() { delete postEffect; delete postEffect2; delete postEffect3; delete postEffect4; };
	void DirectXCommon::Initialize(Window* win)
	{
		win_ = win;

		fixFps_.Initialize();

		InitializeDXGIDevice();
		CreateCommand();
		CreateSwapChain();
		CreateShaderResourceView();
		//CreateRenderTargetView();
		CreateDepthStencilView();
		CreateFence();

		//InitializeImGui();
	}


	void DirectXCommon::ClearRenderTarget(size_t renderTargetNum, const Vector4& clearColor) {
		float clearColorf[] = { clearColor.x,clearColor.y,clearColor.z,clearColor.w };//RGBA
		commandList_->ClearRenderTargetView(rtvHandles_[renderTargetNum], clearColorf, 0, nullptr);
	}
	void DirectXCommon::ResourceBarrier(size_t renderTargetHandle, D3D12_RESOURCE_STATES beforState, D3D12_RESOURCE_STATES afterState) {
		barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier_.Transition.pResource = renderTargetResource_[renderTargetHandle].Get();
		//遷移前
		barrier_.Transition.StateBefore = beforState;
		//遷移後
		barrier_.Transition.StateAfter = afterState;
		//TransitionBarrierを張る
		commandList_->ResourceBarrier(1, &barrier_);
	}

	void DirectXCommon::SetRenderTarget(size_t numRenderTargets, size_t* renderTargetHandles, size_t dsvNum) {
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[kCountOfRenderTarget]{};
		for (size_t index = 0; index < numRenderTargets; index++) {
			rtvHandle[index] = rtvHandles_[renderTargetHandles[index]];
			size_t num = renderTargetHandles[index];
			ResourceBarrier(num, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		}
		if (dsvNum != 0) {
			//仮
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
			commandList_->OMSetRenderTargets(uint32_t(numRenderTargets), rtvHandle, false, &dsvHandle);
		}
		else {
			commandList_->OMSetRenderTargets(uint32_t(numRenderTargets), rtvHandle, false, nullptr);
		}
	}

	void DirectXCommon::SetRenderTarget(size_t renderTargetHandles, size_t dsvNum) {
		size_t renderTargets[1] = { renderTargetHandles };
		SetRenderTarget(1, renderTargets, dsvNum);
	}

	void DirectXCommon::PreDraw()
	{

		ImGui::Render();

		//コマンドの確定
		//UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
		///
		size_t a[] = { 0,1 };
		///
		SetRenderTarget(2, a, 1);
		Vector4 clearColor = { 0.0f,0.0f,0.0f,1.0f };
		ClearRenderTarget(0, clearColor);
		ClearRenderTarget(1, clearColor);
		//指定した深度で画面全体をクリアする
		commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		//ビューポート
		D3D12_VIEWPORT viewport{};
		viewport.Width = FLOAT(win_->GetClientWidth());
		viewport.Height = FLOAT(win_->GetClientHeight());
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;


		//シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0;
		scissorRect.right = LONG(win_->GetClientWidth());
		scissorRect.top = 0;
		scissorRect.bottom = LONG(win_->GetClientHeight());
		commandList_->RSSetViewports(1, &viewport);
		commandList_->RSSetScissorRects(1, &scissorRect);

		ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_.Get() };
		commandList_->SetDescriptorHeaps(1, descriptorHeaps);


	}
	void DirectXCommon::End3DSorceDraw() {
		//posteffect
		ResourceBarrier(0, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		ResourceBarrier(1, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		SetRenderTarget(2, 0);
		ClearRenderTarget(2, Vector4{ 0.0f,0.0f,0.0f,1.0f });

		//バックバッファに書き込む
		postEffect->PreDraw(commandList_.Get());
		postEffect->Draw(srvDescriptorHeap_.Get(), renderSrvHandles_[0], renderSrvHandles_[0]);
		postEffect->PostDraw();

		ResourceBarrier(2, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


		SetRenderTarget(3, 0);
		ClearRenderTarget(3, Vector4{ 0.0f,0.0f,0.0f,1.0f });

		postEffect3->PreDraw(commandList_.Get());
		postEffect3->Draw(srvDescriptorHeap_.Get(), renderSrvHandles_[0], renderSrvHandles_[2]);
		postEffect3->PostDraw();

		//TransitionBarrierを張る
		ResourceBarrier(3, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		SetRenderTarget(2, 0);
		ClearRenderTarget(2, Vector4{ 0.0f,0.0f,0.0f,1.0f });

		//grayscale
		postEffect2->PreDraw(commandList_.Get());
		postEffect2->Draw(srvDescriptorHeap_.Get(), renderSrvHandles_[3], renderSrvHandles_[3]);
		postEffect2->PostDraw();

		ResourceBarrier(2, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
		barrier_.Transition.pResource = swapChainResources_[backBufferIndex].Get();
		//遷移前
		barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		//遷移後
		barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		//TransitionBarrierを張る
		commandList_->ResourceBarrier(1, &barrier_);


		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
		dsvHandle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		commandList_->OMSetRenderTargets(1, &swapChainRTVHandles_[backBufferIndex], false, &dsvHandle);
		float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//RGBA
		commandList_->ClearRenderTargetView(swapChainRTVHandles_[backBufferIndex], clearColor, 0, nullptr);
		//指定した深度で画面全体をクリアする
		commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		//ビューポート
		D3D12_VIEWPORT viewport{};
		viewport.Width = FLOAT(win_->GetClientWidth());
		viewport.Height = FLOAT(win_->GetClientHeight());
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;


		//シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0;
		scissorRect.right = LONG(win_->GetClientWidth());
		scissorRect.top = 0;
		scissorRect.bottom = LONG(win_->GetClientHeight());
		commandList_->RSSetViewports(1, &viewport);
		commandList_->RSSetScissorRects(1, &scissorRect);

		//ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_.Get() };
		//commandList_->SetDescriptorHeaps(1, descriptorHeaps);

		//バックバッファに書き込む
		postEffect4->PreDraw(commandList_.Get());
		postEffect4->Draw(srvDescriptorHeap_.Get(), renderSrvHandles_[2], renderSrvHandles_[2]);
		postEffect4->PostDraw();

	}
	void DirectXCommon::PostDraw()
	{
		UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

#ifdef DEMO
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_.Get());
#endif // _DEBUG
		//描く処理終了、画面に移すため状態を遷移
		//UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
		barrier_.Transition.pResource = swapChainResources_[backBufferIndex].Get();
		barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		//TransitionBarrierを張る
		commandList_->ResourceBarrier(1, &barrier_);

		//コマンドの確定(最後にやる)
		HRESULT hr = commandList_->Close();
		assert(SUCCEEDED(hr));

		//コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(1, commandLists);
		swapChain_->Present(1, 0);

		//Fenceの値をこうしん
		fenceValue_++;
		commandQueue_->Signal(fence_.Get(), fenceValue_);

		//Fenceの値が指定したSignal値にたどり着いているか確認
		if (fence_->GetCompletedValue() < fenceValue_)
		{
			fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
			//イベント待つ
			WaitForSingleObject(fenceEvent_, INFINITE);
		}
		if (isFixed_)
		{
			fixFps_.Update();
		}
		hr = commandAllocator_->Reset();
		assert(SUCCEEDED(hr));
		hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
		assert(SUCCEEDED(hr));

	}

	void DirectXCommon::CreatePostEffectSprite() {
		postEffect = PostEffect::Create({ 0.0f,0.0f }, { 1280.0f,720.0f }, { 1.0f,1.0f,1.0f,1.0f });
		postEffect->Initialize(L"Resources/shaders/BlumeVS.hlsl", L"Resources/shaders/GaussBlumeVertical.PS.hlsl");
		postEffect->SetAnchorPoint({ 0.0f,0.0f });
		postEffect->SetBlendMode(PostEffect::BlendMode::None);

		postEffect2 = PostEffect::Create({ 0.0f,0.0f }, { 1280.0f,720.0f }, { 1.0f,1.0f,1.0f,1.0f });
		postEffect2->Initialize(L"Resources/shaders/BlumeVS.hlsl", L"Resources/shaders/GrayScalePS.hlsl");
		postEffect2->SetAnchorPoint({ 0.0f,0.0f });
		postEffect2->SetBlendMode(PostEffect::BlendMode::None);

		postEffect3 = PostEffect::Create({ 0.0f,0.0f }, { 1280.0f,720.0f }, { 1.0f,1.0f,1.0f,1.0f });
		postEffect3->Initialize(L"Resources/shaders/BlumeVS.hlsl", L"Resources/shaders/GaussBlumeHorizon.PS.hlsl");
		postEffect3->SetAnchorPoint({ 0.0f,0.0f });
		postEffect3->SetBlendMode(PostEffect::BlendMode::None);

		postEffect4 = PostEffect::Create({ 0.0f,0.0f }, { 1280.0f,720.0f }, { 1.0f,1.0f,1.0f,1.0f });
		postEffect4->Initialize(L"Resources/shaders/BlumeVS.hlsl", L"Resources/shaders/HSVFilter.PS.hlsl");
		postEffect4->SetAnchorPoint({ 0.0f,0.0f });
		postEffect4->SetBlendMode(PostEffect::BlendMode::None);
		postEffect4->SetColor({ 0.0f,0.0f,0.0f,1.0f });
	}

	void DirectXCommon::InitializeDXGIDevice()
	{
		//DXGIファクトリーの生成

		HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
		assert(SUCCEEDED(hr));

		//アダプタ関係
		//IDXGIAdapter4* useAdapter_ = nullptr;
		for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
			DXGI_ERROR_NOT_FOUND; i++)
		{
			DXGI_ADAPTER_DESC3 adapterDesc{};
			hr = useAdapter_->GetDesc3(&adapterDesc);
			assert(SUCCEEDED(hr));
			//ソフトウェアアダプタで無ければ
			if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
			{
				Log(ConvertString(std::format(L"USE Adapter:{}\n", adapterDesc.Description)));
				break;
			}
			useAdapter_ = nullptr;
		}
		assert(useAdapter_ != nullptr);

		//D3D12Deviceの生成
		device_ = nullptr;
		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0 };
		const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
		for (size_t i = 0; i < _countof(featureLevels); ++i)
		{
			hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
			if (SUCCEEDED(hr))
			{
				Log(std::format("FeatureLevel:{}\n", featureLevelStrings[i]));
				break;
			}
		}
		assert(device_ != nullptr);
		Log("Complete create D3D12Device!!!\n");

#ifdef _DEBUG
		ID3D12InfoQueue* infoQueue = nullptr;
		if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue))))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			infoQueue->Release();

			D3D12_MESSAGE_ID denyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
				D3D12_MESSAGE_ID_UNKNOWN
			};

			//抑制するレベル
			D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
			D3D12_INFO_QUEUE_FILTER filter{};
			filter.DenyList.NumIDs = _countof(denyIds);
			filter.DenyList.pIDList = denyIds;
			filter.DenyList.NumSeverities = _countof(severities);
			filter.DenyList.pSeverityList = severities;
			//指定したメッセージの表示を抑制
			infoQueue->PushStorageFilter(&filter);
		}
#endif // _DEBUG

	}

	void DirectXCommon::CreateCommand()
	{
		//コマンドキューの生成
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
		HRESULT hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
		commandQueue_->SetName(L"CommandQueue");
		//コマンドキューの生成に失敗
		assert(SUCCEEDED(hr));

		//コマンドアロケータの生成
		hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
		commandAllocator_->SetName(L"CommandAllocator");
		//コマンドアロケータの生成に失敗
		assert(SUCCEEDED(hr));

		//コマンドリストの生成
		hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
		//コマンドリストの生成に失敗
		assert(SUCCEEDED(hr));
	}

	void DirectXCommon::CreateSwapChain()
	{
		//スワップチェーンの生成
		swapChainDesc_.Width = win_->GetClientWidth();
		swapChainDesc_.Height = win_->GetClientHeight();
		swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc_.SampleDesc.Count = 1;
		swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc_.BufferCount = 2;
		swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		HRESULT hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), win_->GetHwnd(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
		assert(SUCCEEDED(hr));
	}

	void DirectXCommon::CreateRenderTargetView()
	{
		//RTV用のヒープでディスクリプタの数は2
		rtvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kCountOfRenderTarget + 2, false);

		//SwapChainからResourceをひっぱってくる

		HRESULT hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
		assert(SUCCEEDED(hr));
		hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
		assert(SUCCEEDED(hr));

		///RTVの作成
		//RTVの設定
		rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//ディスクリプタの先頭取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
		//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
		//1つ目
		swapChainRTVHandles_[0] = rtvStartHandle;
		device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, swapChainRTVHandles_[0]);
		//2つ目
		swapChainRTVHandles_[1].ptr = swapChainRTVHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, swapChainRTVHandles_[1]);

		//3つ目
		//swapchain以外のrenderTargetの分ポインタを格納する
		rtvHandles_[0].ptr = swapChainRTVHandles_[1].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for (uint32_t index = 1; index < kCountOfRenderTarget; index++) {
			rtvHandles_[index].ptr = rtvHandles_[index - 1].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;
		//リソースの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		//バッファリソース。テクスチャの場合は別の設定をする
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = 1280;
		resourceDesc.Height = 720;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 0;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Format = rtvDesc_.Format;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		//リソースを作る
		ID3D12Resource* resourse = nullptr;
		const float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		clearValue.Color[0] = clearColor[0];
		clearValue.Color[1] = clearColor[1];
		clearValue.Color[2] = clearColor[2];
		clearValue.Color[3] = clearColor[3];
		for (size_t index = 0; index < kCountOfRenderTarget; index++) {
			hr = device_->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clearValue, IID_PPV_ARGS(&resourse));
			renderTargetResource_[index] = resourse;
			srvPostEffectHandle = SRVManager::GetInstance()->CreateSRV(renderTargetResource_[index].Get(), &srvDesc);
			device_->CreateRenderTargetView(renderTargetResource_[index].Get(), &rtvDesc_, rtvHandles_[index]);
			renderSrvHandles_[index] = SRVManager::GetInstance()->GetGPUHandle(srvPostEffectHandle);
		}

	}

	void DirectXCommon::CreateShaderResourceView()
	{
		srvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kNumSrvDescriptors, true);
		SRVManager::GetInstance()->Initialize(device_.Get(), srvDescriptorHeap_.Get());
	}

	void DirectXCommon::CreateDepthStencilView()
	{
		//DepthStencilTextureをウィンドウのサイズで作成
		depthStencilResource_ = CreateDepthStencilTextureResource(device_.Get(), win_->GetClientWidth(), win_->GetClientHeight());
		depthStencilResource2_ = CreateDepthStencilTextureResource(device_.Get(), win_->GetClientWidth(), win_->GetClientHeight());
		//DSV用のヒープでディスクリプタの数は1,DSVはShader内で触るものではないので.ShaderVisibleはfalse
		dsvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 2, false);
		//DSVの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		//DSVHeapの先頭にDSVを作る
		device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
		D3D12_CPU_DESCRIPTOR_HANDLE handle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		device_->CreateDepthStencilView(depthStencilResource2_.Get(), &dsvDesc, handle);
	}

	void DirectXCommon::CreateFence()
	{
		//初期値0でFenceを作る
		fenceValue_ = 0;
		HRESULT hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
		assert(SUCCEEDED(hr));
		//Fence Signalを待つためのイベントを作成する
		fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent_ != nullptr);
	}

	void DirectXCommon::InitializeImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(win_->GetHwnd());
		ImGui_ImplDX12_Init(device_.Get(), swapChainDesc_.BufferCount, rtvDesc_.Format, srvDescriptorHeap_.Get(),
			srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(), srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart());
		//コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList_.Get() };
		commandList_->Close();
		commandQueue_->ExecuteCommandLists(1, commandLists);

		//Fenceの値をこうしん
		fenceValue_++;
		commandQueue_->Signal(fence_.Get(), fenceValue_);

		//Fenceの値が指定したSignal値にたどり着いているか確認
		if (fence_->GetCompletedValue() < fenceValue_)
		{
			fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
			//イベント待つ
			WaitForSingleObject(fenceEvent_, INFINITE);
		}

		HRESULT hr = commandAllocator_->Reset();
		assert(SUCCEEDED(hr));
		hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
		assert(SUCCEEDED(hr));
	}
	/*
	DirectX::ScratchImage InputTexture(const std::string& filePath)
	{
		DirectX::ScratchImage image{};
		std::wstring filePathW = ConvertString(filePath);
		HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));

		DirectX::ScratchImage mipImages{};
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		assert(SUCCEEDED(hr));

		return mipImages;
	}
	*/
	ID3D12Resource* DirectXCommon::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
	{
		//metadataをもとにのResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = UINT(metadata.width);
		resourceDesc.Height = UINT(metadata.height);
		resourceDesc.MipLevels = UINT16(metadata.mipLevels);
		resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
		resourceDesc.Format = metadata.format;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

		//利用するヒープの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

		//Resourceの生成
		ID3D12Resource* resource = nullptr;
		HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource));
		assert(SUCCEEDED(hr));
		return resource;
	}

	//Resource作成

	ID3D12Resource* DirectXCommon::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
	{
		//リソース用のヒープの設定
		D3D12_HEAP_PROPERTIES uploadHeapProperties{};
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		//リソースの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		//バッファリソース。テクスチャの場合は別の設定をする
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Width = sizeInBytes;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		//リソースを作る
		ID3D12Resource* resourse = nullptr;
		HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resourse));
		assert(SUCCEEDED(hr));
		resourse->SetName(L"Buffer");
		return resourse;
	}

	[[nodiscard]]
	ID3D12Resource* CreateIntermadiateResource(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
	{
		std::vector<D3D12_SUBRESOURCE_DATA> subresource;
		DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresource);
		uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresource.size()));
		ID3D12Resource* intermediateResource = DirectXCommon::CreateBufferResource(device, intermediateSize);
		UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresource.size()), subresource.data());
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = texture;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		commandList->ResourceBarrier(1, &barrier);
		return intermediateResource;
	}

	void  DirectXCommon::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
	{
		ID3D12Resource* intermediateResource = CreateIntermadiateResource(texture, mipImages, device_.Get(), commandList_.Get());

		//コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList_.Get() };
		commandList_->Close();
		commandQueue_->ExecuteCommandLists(1, commandLists);

		//Fenceの値をこうしん
		fenceValue_++;
		commandQueue_->Signal(fence_.Get(), fenceValue_);

		//Fenceの値が指定したSignal値にたどり着いているか確認
		if (fence_->GetCompletedValue() < fenceValue_)
		{
			fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
			//イベント待つ
			WaitForSingleObject(fenceEvent_, INFINITE);
		}

		HRESULT hr = commandAllocator_->Reset();
		assert(SUCCEEDED(hr));
		hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
		assert(SUCCEEDED(hr));
		intermediateResource->Release();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += (descriptorSize * index);
		return handleCPU;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += (descriptorSize * index);
		return handleGPU;
	}
	/*
	int32_t DirectXCommon::LoadTexture(const std::string& filePath)
	{

		DirectX::ScratchImage* mipImages = new DirectX::ScratchImage();
		*mipImages = InputTexture("Resources/uvChecker.png");
		const DirectX::TexMetadata& metadata = mipImages->GetMetadata();
		ID3D12Resource* textureResource = CreateTextureResource(device_, metadata);
		ID3D12Resource* intermediateResource = UploadTextureData(textureResource, *mipImages, device_, commandList_);

		//コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList_ };
		commandList_->Close();
		commandQueue_->ExecuteCommandLists(1, commandLists);

		//Fenceの値をこうしん
		fenceValue_++;
		commandQueue_->Signal(fence_, fenceValue_);

		//Fenceの値が指定したSignal値にたどり着いているか確認
		if (fence_->GetCompletedValue() < fenceValue_)
		{
			fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
			//イベント待つ
			WaitForSingleObject(fenceEvent_, INFINITE);
		}

		HRESULT hr = commandAllocator_->Reset();
		assert(SUCCEEDED(hr));
		hr = commandList_->Reset(commandAllocator_, nullptr);
		assert(SUCCEEDED(hr));
		intermediateResource->Release();

		mipImages_.push_back(mipImages);
		//metadatas_.push_back(metadata);
		textureResources_.push_back(textureResource);

		//metadataを基にSRVの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

		//SRVを作成するDescriptorHeapの場所を決める
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV_, textureCount_);
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV_, textureCount_);

		//textureSrvHandleCPU.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//textureSrvHandleGPU.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//SRVの生成
		device_->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);

		return textureCount_++;

	}
	*/

	void DirectXCommon::SetGraiScaleStrength(float strength) { postEffect2->SetColor({ 1.0f,1.0f,1.0f,strength }); };

	void DirectXCommon::SetHSVFilter(const Vector4& filter) {
		postEffect4->SetColor(filter);
	};
}