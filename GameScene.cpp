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

#include "collision.h"

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

	particle.reset(Particle::Create(uvCheckerTextureHandle_,10));

	sprite_.reset(Sprite::Create(uvCheckerTextureHandle_, { 0,0 }, { 720,360 }, {1.0f,1.0f,1.0f,1.0f}));
	sprite_->SetAnchorPoint({0,0});
	spritePosition_ = {0,0};
	ancorPoint_ = {0,0};
	rotate_ = 0;
	leftTop = {0,0};
	rightDown = {720.0f,360.0f};
}

void GameScene::Update() {
#ifdef _DEBUG
	//デバッグカメラを有効化
	if (Input::GetKeyDown(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
		debugCamera_->SetUses(isDebugCameraActive_);
	}
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
	sprite_->Draw();
}

void GameScene::Draw3D() {
/*	Primitive3D::PreDraw(dxCommon_->GetCommandList());
	Primitive3D::PostDraw();*/
	Model::PreDraw(dxCommon_->GetCommandList());
	//skydome_->Draw(viewProjection_);
	//ground_->Draw(viewProjection_);
	//flooar_->Draw(viewProjection_);
	
	Model::PostDraw();
	Particle::PreDraw(dxCommon_->GetCommandList());
	//particle->Draw(viewProjection_);
	Particle::PostDraw();
}

