#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <list>
#include <fstream>
#include  "Primitive3D.h"
#include "Input.h"
//ImGui
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include <cassert>
#include <list>
#include <numbers>
#include "collision.h"
#include "CollisionManager.h"
#include "RandomEngine.h"
#include "DeltaTime.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include "RandomEngine.h"
#include "Engine/Audio/AudioManager.h"
#include "GetDescriptorHandle.h"
#include "ConvertString.h"
GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;
	input_ = Input::GetInstance();

	debugCamera_.reset(new DebugCamera);
	debugCamera_->Initialize();

	//audioHandle_ = AudioManager::GetInstance()->LoadWave("Alarm01.wav");
	//audioHandle_ = AudioManager::GetInstance()->LoadAudio("1.mp3");
	//AudioManager::GetInstance()->PlayWave(audioHandle_);

	
	sceneLoader_.reset(new SceneLoader);
	sceneLoader_->LoadFile("testScene");
	modelList_.clear();
	sceneLoader_->CreateModelList(modelList_);
	objects_.clear();
	sceneLoader_->CreateObjects(objects_);
#ifdef _DEBUG
	isDebugCameraActive_ = true;
	debugCamera_->SetUses(isDebugCameraActive_);
	//debugCamera_->SetRotate({ std::numbers::pi_v<float> / 3.0f,std::numbers::pi_v<float> ,0.0f });
	debugCamera_->SetPosition({ 0.0f, 1.7f, -10.0f });
#endif // _DEBUG
	
	//srvの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = 32;
	srvDesc.Buffer.StructureByteStride = sizeof(PointLight);

	pointLightResource = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(PointLight) * pointLightMax_);

	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));
	for (uint32_t index = 0; index < pointLightMax_; ++index) {
		pointLightData[index].color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
		pointLightData[index].position = { 0.0f,1.0f,0.0f };
		pointLightData[index].intensity = 1.0f;
		pointLightData[index].radius = 10.0f;
		pointLightData[index].decay = 0.0f;
		pointLightData[index].isUse_ = 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU = GetCPUDescriptorHandle(dxCommon_->GetsrvDescriptorHeap(), dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), uint32_t(512));
	srvHandleGPU = GetGPUDescriptorHandle(dxCommon_->GetsrvDescriptorHeap(), dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), uint32_t(512));

	dxCommon_->GetDevice()->CreateShaderResourceView(pointLightResource.Get(), &srvDesc, srvHandleCPU);

	//PointLight用のリソース
	//pointLightResource = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(), sizeof(PointLight));
	//pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));
//	pointLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	//pointLightData->position = { 0.0f,1.0f,0.0f };
	//pointLightData->intensity = 1.0f;
	//pointLightData->radius = 35.0f;
	//pointLightData->decay = 1.0f;

	//PointLight用のリソース
	spotLightResource = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(), sizeof(SpotLight));
	spotLightResource->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));
	spotLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	spotLightData->position = { 2.0f,1.25f,0.0f };
	spotLightData->intensity = 0.0f;
	spotLightData->direction = Normalize(Vector3{-1.0f,-1.0f,0.0f});
	spotLightData->distance = 7.0f;
	spotLightData->decay = 2.0f;
	spotLightData->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);

	//skybox
	skyBox_.reset(new SkyBox);
	skyBox_->Initialize(TextureManager::LoadTexture("rostock_laage_airport_4k.dds"));
	worldTransformSkyBox_.Initialize();
	worldTransformSkyBox_.scale_ = { 1000.0f,1000.0f,1000.0f };
	worldTransformSkyBox_.UpdateMatrix();

	CollisionManager::GetInstance()->ClearList();

	CollisionManager::GetInstance();
	colorPhase_ = 0;
	color_ = 0.0f;

	grayScaleValue_ = 0.0f;
	dxCommon_->SetGraiScaleStrength(grayScaleValue_);

	viewProjection_.Initialize();
}

void GameScene::Update() {
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

#ifdef _DEBUG
	if (Input::GetKeyDown(DIK_RSHIFT)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
		debugCamera_->SetUses(isDebugCameraActive_);
	}
#endif // _DEBUG

	for (std::unique_ptr<GameObject>& object:objects_) {
		object->Update();
	}

	if (isDebugCameraActive_) {
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.translation_ = debugCamera_->GetViewProjection().translation_;
		//viewProjection_.TransferMatrix();
	}
	debugCamera_->Update();
}

void GameScene::Draw2D() {

}

void GameScene::Draw3D() {
	//Primitive3D::PreDraw(dxCommon_->GetCommandList());
	//Primitive3D::PostDraw();
	Model::PreDraw(dxCommon_->GetCommandList());
	
	for (std::unique_ptr<GameObject>& object : objects_) {
		object->Draw(viewProjection_,modelList_);
	}

	Model::PostDraw();
	Model::PreDrawOutLine(dxCommon_->GetCommandList());
	Model::PostDraw();

	//skybox描画
	skyBox_->Draw(worldTransformSkyBox_,viewProjection_);

}
