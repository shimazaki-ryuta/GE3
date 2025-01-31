#include "GameManager.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>
#pragma comment(lib,"winmm.lib")

#include <cstdint>
#include <string>
#include <format>
#include <vector>
#include <memory>
#include"ConvertString.h"
#include "2D/PostEffect.h"

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


const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;
const std::string kTitle = "";


void GameManager::Initialize() {
	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	mainWindow_ = new Window();
	mainWindow_->CreateGameWindow(kTitle, kClientWidth, kClientHeight);


#ifdef _DEBUG
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_))))
	{
		debugController_->EnableDebugLayer();
		//debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif 

	dxCommon_ = new DirectXCommon();
	dxCommon_->Initialize(mainWindow_);
	dxCommon_->FixedFPS(true);

	//入力関数の初期化
	Input::GetInstance()->Initialize(mainWindow_->GetHwnd());

	//TextureManagerの初期化
	TextureManager* textureManager = TextureManager::GetInstance();
	textureManager->Initialize(dxCommon_->GetDevice());
	textureManager->SetDirectXCommon(dxCommon_);
	textureManager->SetsrvDescriptorHeap(dxCommon_->GetsrvDescriptorHeap());
	dxCommon_->CreateRenderTargetView();
	dxCommon_->InitializeImGui();

	//worldTransformの初期化
	WorldTransform::SetDevice(dxCommon_->GetDevice());
	Animation::SetDevice(dxCommon_->GetDevice());


	//Spriteの初期化
	Sprite::StaticInitialize(dxCommon_->GetDevice());
	PostEffect::SetDevice(dxCommon_->GetDevice());
	dxCommon_->CreatePostEffectSprite();
	//Modelの初期化
	LoadModel::SInitialize(dxCommon_->GetDevice(), dxCommon_->GetsrvDescriptorHeap());
	Material::SetDevice(dxCommon_->GetDevice());
	Model::StaticInitialize(dxCommon_->GetDevice());
	Particle::StaticInitialize(dxCommon_->GetDevice(), dxCommon_->GetsrvDescriptorHeap());
	SkyBox::StaticInitialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());
	

	//音関係初期化
	AudioManager::GetInstance()->Initialize();

	const uint32_t descriptorSizeSRV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dxCommon_->SetDescriptorSizeSRV(descriptorSizeSRV);
	const uint32_t descriptorSizeRTV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dxCommon_->SetDescriptorSizeRTV(descriptorSizeRTV);
	const uint32_t descriptorSizeDSV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	dxCommon_->SetDescriptorSizeDSV(descriptorSizeDSV);

	//シーンファクトリー
	sceneFactory_ = std::make_unique<SceneFactory>();

	//デフォルトシーンの読み込み
	scene_.reset(sceneFactory_->CreateScene());
	

	//グローバル変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();
}

void GameManager::Run() {
	MSG msg{};
	DeltaTime::GameLoopStart();

	scene_->Initialize(dxCommon_);

	while (!mainWindow_->ProcessMessage())
	{
		
		//更新処理開始
		Input::GetInstance()->KeyboardUpdate();
		DeltaTime::FrameStart();
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		GlobalVariables::GetInstance()->Update();
		scene_->Update();

		//描画
		dxCommon_->PreDraw();
			

		scene_->Draw3D();

		dxCommon_->End3DSorceDraw();
			
		Sprite::PreDraw(dxCommon_->GetCommandList());
		scene_->Draw2D();
		Sprite::PostDraw();
			
		dxCommon_->PostDraw();
		
	}
	dxCommon_->DeletePostEffect();
	Log(ConvertString(std::format(L"WSTRING:{}\n", msg.message)));

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}