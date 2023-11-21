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
#include "DeltaTime.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;
	input_ = Input::GetInstance();

	debugCamera_.reset(new DebugCamera);
	debugCamera_->Initialize();
	viewProjection_.Initialize();

	// 天球
	modelSkydome_ = Model::CreateFromOBJ("skydome");
	skydome_.reset(new Skydome);
	skydome_->Initialize(modelSkydome_, Vector3(0.0f, 0.0f, 0.0f));

	// 地面
	modelGround_ = Model::CreateFromOBJ("Ground");
	ground_.reset(new Ground);
	ground_->Initialize(modelGround_, Vector3(0.0f, 0.0f, 0.0f));

	uvCheckerTextureHandle_ = TextureManager::LoadTexture("uvChecker.png");
	monsterTextureHandle_ = TextureManager::LoadTexture("monsterBall.png");
	//particle.reset(Particle::Create(500));

	sprite_.reset(Sprite::Create(uvCheckerTextureHandle_, { 0,0 }, { 720,360 }, { 1.0f,1.0f,1.0f,1.0f }));
	sprite_->SetAnchorPoint({ 0,0 });
	spritePosition_ = { 0,0 };
	ancorPoint_ = { 0,0 };
	rotate_ = 0;
	leftTop = { 0,0 };
	rightDown = { 720.0f,360.0f };
#ifdef _DEBUG
	isDebugCameraActive_ = true;
	debugCamera_->SetUses(isDebugCameraActive_);
	//debugCamera_->SetRotate({ std::numbers::pi_v<float> / 3.0f,std::numbers::pi_v<float> ,0.0f });
	debugCamera_->SetPosition({0.0f, 0.0f, -10.0f});
#endif // _DEBUG
	
	//DirectionalLight用のリソース
	directinalLightResource = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(), sizeof(DirectionalLight));
	directinalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directinalLightData));
	directinalLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	directinalLightData->direction = { 0.0f,-1.0f,0.0f };
	directinalLightData->intensity = 1.0f;
	sphere_.reset(Model::CreateFromOBJ("uvSphere"));
	sphere_->SetEnableLighting(2);
	worldTransformSphere_.Initialize();
}

void GameScene::Update() {
#ifdef _DEBUG
	//デバッグカメラを有効化
	if (Input::GetKeyDown(DIK_RSHIFT)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
		debugCamera_->SetUses(isDebugCameraActive_);
	}

	ImGui::Begin("DirectionalLight");
	ImGui::SliderFloat3("DirectionalLight", &directinalLightData->direction.x, -1.0f, 1.0f, 0);
	directinalLightData->direction = Normalize(directinalLightData->direction);
	ImGui::SliderFloat("Intensity", &directinalLightData->intensity, 0.0f, 1.0f, 0);
	ImGui::ColorEdit4("DirectionalLightColor", &directinalLightData->color.x);
	ImGui::DragFloat("Shininess",&shininess_,0.001f,0.0f,200.0f);
	ImGui::End();
	sphere_->SetShiniess(shininess_);
	ImGui::Begin("Sprite");
	ImGui::DragFloat2("position",&spritePosition_.x,1.0f);
	ImGui::DragFloat2("anchor", &ancorPoint_.x, 0.1f);
	ImGui::DragFloat("rotate",&rotate_,0.1f);
	ImGui::DragFloat2("leftTop", &leftTop.x, 1.0f);
	ImGui::DragFloat2("rightDown", &rightDown.x, 1.0f);
	ImGui::End();
	sprite_->SetPosition(spritePosition_);
	sprite_->SetAnchorPoint(ancorPoint_);
	sprite_->SetRotate(rotate_);
	sprite_->SetRange(leftTop,rightDown);
	
#endif // _DEBUG
	
	
	if (isDebugCameraActive_) {
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.translation_ = debugCamera_->GetViewProjection().translation_;
		//viewProjection_.TransferMatrix();
	}
	else {
		// viewProjection_.UpdateMatrix();
		//viewProjection_.matView = followCamera_->GetViewProjection().matView;
		//viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		//viewProjection_.TransferMatrix();
	}
	debugCamera_->Update();
}

void GameScene::Draw2D() {
	//sprite_->Draw();
}

void GameScene::Draw3D() {
	//Primitive3D::PreDraw(dxCommon_->GetCommandList());
	//Primitive3D::PostDraw();
	Model::PreDraw(dxCommon_->GetCommandList());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	sphere_->Draw(worldTransformSphere_, viewProjection_,monsterTextureHandle_);
	//skydome_->Draw(viewProjection_);
	//ground_->Draw(viewProjection_);
	//flooar_->Draw(viewProjection_);
	
	Model::PostDraw();
	Particle::PreDraw(dxCommon_->GetCommandList());
	Particle::PostDraw();
}

