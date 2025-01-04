#include "GameManager.h"

#define _USE_MATH_DEFINES
#include <math.h>
//#include "WindowProcedure.h"
#include <cstdint>
#include <string>
#include <format>
#include <vector>
#include <memory>
#include"ConvertString.h"
#include "2D/PostEffect.h"
//static std::shared_ptr<D3DResourceLeakChacker> leakchecker;

//Shader関係
#include <dxcapi.h>
//描画関係
#include "TextureManager.h"
#include "DeltaTime.h"
//#include "Vector4.h"
#include "2D/Sprite.h"
//#include "3D/Primitive3D.h"
#include "3D/SkyBox.h"
//入力
#include "Input.h"
//ImGui
#include "../externals/imgui/imgui.h"
#include "../externals/imgui/imgui_impl_dx12.h"
#include "../externals/imgui/imgui_impl_win32.h"

#include "3D/WorldTransform.h"

//音
#include "Audio/AudioManager.h"

//ファイル書き出し
#include "GlobalVariables/GlobalVariables.h"

#include "3D/Animation.h"

#include "3D/LoadModel.h"
#include "3D/Particle.h"

#include "../Application/GameScene.h"

const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;
const std::string kTitle = "";


void GameManager::Initialize() {
	mainWindow = new Window();
	mainWindow->CreateGameWindow(kTitle, kClientWidth, kClientHeight);


#ifdef _DEBUG
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		//debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif 

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
	dxCommon->CreateRenderTargetView();
	dxCommon->InitializeImGui();

	//worldTransformの初期化
	WorldTransform::SetDevice(dxCommon->GetDevice());
	Animation::SetDevice(dxCommon->GetDevice());


	//Spriteの初期化
	Sprite::StaticInitialize(dxCommon->GetDevice());
	PostEffect::SetDevice(dxCommon->GetDevice());
	//PostEffect::StaticInitialize(dxCommon->GetDevice(), mainWindow->GetClientWidth(), mainWindow->GetClientHeight());
	dxCommon->CreatePostEffectSprite();
	//Modelの初期化
	LoadModel::SInitialize(dxCommon->GetDevice(), dxCommon->GetsrvDescriptorHeap());
	Material::SetDevice(dxCommon->GetDevice());
	Model::StaticInitialize(dxCommon->GetDevice());
	Particle::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetsrvDescriptorHeap());
	//Primitive3D::StaticInitialize(dxCommon->GetDevice(), mainWindow->GetClientWidth(), mainWindow->GetClientHeight());
	SkyBox::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());
	//DeltaTime::GetInstance();

	//音関係初期化
	AudioManager::GetInstance()->Initialize();

	const uint32_t descriptorSizeSRV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dxCommon->SetDescriptorSizeSRV(descriptorSizeSRV);
	const uint32_t descriptorSizeRTV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dxCommon->SetDescriptorSizeRTV(descriptorSizeRTV);
	const uint32_t descriptorSizeDSV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	dxCommon->SetDescriptorSizeDSV(descriptorSizeDSV);

	scene = new GameScene;
	//gameScene.reset(new GameScene);
	//gameScene->Initialize(dxCommon);

	//グローバル変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();
}

void GameManager::Run() {
	MSG msg{};
	DeltaTime::GameLoopStart();

	scene->Initialize(dxCommon);

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
			scene->Update();

			//描画
			dxCommon->PreDraw();
			

			scene->Draw3D();

			dxCommon->End3DSorceDraw();
			
			Sprite::PreDraw(dxCommon->GetCommandList());
			scene->Draw2D();
			Sprite::PostDraw();
			
			dxCommon->PostDraw();
		}
	}
	dxCommon->DeletePostEffect();
	Log(ConvertString(std::format(L"WSTRING:{}\n", msg.message)));

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	delete scene;
}