//メモリリークチェック
#include "D3DResourceLeakChacker.h"
//static D3DResourceLeakChacker* leacCheck = D3DResourceLeakChacker::GetInstance();
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
//#include "WindowProcedure.h"
#include <cstdint>
#include <string>
#include <format>
#include <vector>
#include"ConvertString.h"

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

const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;
const std::string kTitle = "";


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//D3DResourceLeakChacker leacCheck;// = D3DResourceLeakChacker::GetInstance();
	assert(SUCCEEDED(CoInitializeEx(0,COINIT_MULTITHREADED)));

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

	//入力関数の初期化
	Input::GetInstance()->Initialize(mainWindow->GetHwnd());

	//TextureManagerの初期化
	TextureManager* textureManager = TextureManager::GetInstance();
	textureManager->Initialize(dxCommon->GetDevice());
	textureManager->SetDirectXCommon(dxCommon);
	textureManager->SetsrvDescriptorHeap(dxCommon->GetsrvDescriptorHeap());

	//Spriteの初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(),mainWindow->GetClientWidth(), mainWindow->GetClientHeight());

	Primitive3D::StaticInitialize(dxCommon->GetDevice(), mainWindow->GetClientWidth(), mainWindow->GetClientHeight());

	//DeltaTime::GetInstance();

	const uint32_t descriptorSizeSRV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dxCommon->SetDescriptorSizeSRV(descriptorSizeSRV);
	const uint32_t descriptorSizeRTV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dxCommon->SetDescriptorSizeRTV(descriptorSizeRTV);
	const uint32_t descriptorSizeDSV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	dxCommon->SetDescriptorSizeDSV(descriptorSizeDSV);
	
	GameScene* gameScene = new GameScene();
	gameScene->Initialize(dxCommon);
	
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


			//描画
			dxCommon->PreDraw();
		
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