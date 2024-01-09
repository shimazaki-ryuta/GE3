//メモリリークチェック
#include "D3DResourceLeakChacker.h"
LeackChecker leackChecker;
//static D3DResourceLeakChacker* leacCheck = D3DResourceLeakChacker::GetInstance();
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
//#include "WindowProcedure.h"
#include <cstdint>
#include <string>
#include <format>
#include <vector>
#include <memory>
#include"ConvertString.h"

//static std::shared_ptr<D3DResourceLeakChacker> leakchecker;

//window関係
#include "CommonFiles/WinApp.h"
//directx12関係
#include "CommonFiles/DirectXCommon.h"
//Shader関係
#include <dxcapi.h>
//描画関係
#include "TextureManager.h"
#include "DeltaTime.h"
//#include "Vector4.h"
#include "Sprite.h"
#include "Primitive3D.h"
//入力
#include "Input.h"

#include "GameScene.h"
//ImGui
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

#include "WorldTransform.h"

//ファイル書き出し
#include "GlobalVariables.h"

const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;
const std::string kTitle = "LE2A_11_シマザキ_リュウタ_DOPPELSHOTER";

struct DirectionalLight
{
	Vector4 color;
	Vector3 direction;
	float intensity;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//D3DResourceLeakChacker leacCheck;// = D3DResourceLeakChacker::GetInstance();
	assert(SUCCEEDED(CoInitializeEx(0,COINIT_MULTITHREADED)));
	//std::shared_ptr<D3DResourceLeakChacker> leakchecker;
	//D3DResourceLeakChacker* leakchecker = D3DResourceLeakChacker::GetInstance();
	//leakchecker.reset(D3DResourceLeakChacker::GetInstance());
	///初期化
	Window* mainWindow=nullptr;
	mainWindow = new Window();
	mainWindow->CreateGameWindow(kTitle,kClientWidth,kClientHeight);
	

#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif 

	DirectXCommon* dxCommon = nullptr;
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(mainWindow);
	dxCommon->FixedFPS(true);

	//入力関数の初期化
	Input::GetInstance()->Initialize(mainWindow->GetHwnd());

	//TextureManagerの初期化
	TextureManager* textureManager = TextureManager::GetInstance();
	textureManager->Initialize(dxCommon->GetDevice());
	textureManager->SetDirectXCommon(dxCommon);
	textureManager->SetsrvDescriptorHeap(dxCommon->GetsrvDescriptorHeap());

	//worldTransformの初期化
	WorldTransform::SetDevice(dxCommon->GetDevice());

	//Spriteの初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(),mainWindow->GetClientWidth(), mainWindow->GetClientHeight());

	//Modelの初期化
	Model::StaticInitialize(dxCommon->GetDevice(), mainWindow->GetClientWidth(), mainWindow->GetClientHeight());

	Primitive3D::StaticInitialize(dxCommon->GetDevice(), mainWindow->GetClientWidth(), mainWindow->GetClientHeight());

	Particle::StaticInitialize(dxCommon->GetDevice(),dxCommon->GetsrvDescriptorHeap());

	//DeltaTime::GetInstance();

	const uint32_t descriptorSizeSRV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dxCommon->SetDescriptorSizeSRV(descriptorSizeSRV);
	const uint32_t descriptorSizeRTV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dxCommon->SetDescriptorSizeRTV(descriptorSizeRTV);
	const uint32_t descriptorSizeDSV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	dxCommon->SetDescriptorSizeDSV(descriptorSizeDSV);
	
	GameScene* gameScene = new GameScene();
	gameScene->Initialize(dxCommon);
	
	//DirectionalLight用のリソース
	ID3D12Resource* directinalLightResource = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(), sizeof(DirectionalLight));
	DirectionalLight* directinalLightData = nullptr;
	directinalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directinalLightData));
	directinalLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	directinalLightData->direction = { 0.0f,-1.0f,0.0f };
	directinalLightData->intensity = 1.0f;

	//グローバル変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();

	///メインループ
	MSG msg{};
	DeltaTime::GameLoopStart();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//更新処理開始
			Input::GetInstance()->KeyboardUpdate();
			DeltaTime::FrameStart();
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();			
			GlobalVariables::GetInstance()->Update();
			gameScene->Update();

			//描画
			dxCommon->PreDraw();
		
			//Lighting用のリソースの場所を設定
			//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());

			gameScene->Draw3D();
		

			Sprite::PreDraw(dxCommon->GetCommandList());
			gameScene->Draw2D();
			Sprite::PostDraw();

			dxCommon->PostDraw();
		}
	}

	Log(ConvertString(std::format(L"WSTRING:{}\n",msg.message)));

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	//解放処理
	delete dxCommon;
#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG
	CloseWindow(mainWindow->GetHwnd());

	CoUninitialize();

	return 0;
}