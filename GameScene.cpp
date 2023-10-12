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

GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;
	input_ = Input::GetInstance();


	// 3Dモデルデータの生成
	//model_.reset(Model::CreateFromOBJ("Player", true));
	modelPlayerBody_.reset(Model::CreateFromOBJ("float_Body"));
	modelPlayerHead_.reset(Model::CreateFromOBJ("float_Head"));
	modelPlayerL_arm_.reset(Model::CreateFromOBJ("float_L_arm"));
	modelPlayerR_arm_.reset(Model::CreateFromOBJ("float_R_arm"));
	// std::vector<Model*> modelPlayers_;
	std::vector<HierarchicalAnimation> animationPlayer;
	WorldTransform worldTransformPlayerBody = {
		.translation_{0.0f, 0.0f, 0.0f}
	};
	WorldTransform worldTransformPlayerHead = {
		.translation_{0.0f, 1.2f, 0.0f}
	};
	WorldTransform worldTransformPlayerL_arm = {
		.translation_{-0.5f, 1.5f, 0.0f}
	};
	WorldTransform worldTransformPlayerR_arm = {
		.translation_{0.5f, 1.5f, 0.0f}
	};

	animationPlayer.push_back({ modelPlayerBody_.get(), worldTransformPlayerBody });
	animationPlayer.push_back({ modelPlayerHead_.get(), worldTransformPlayerHead });
	animationPlayer.push_back({ modelPlayerL_arm_.get(), worldTransformPlayerL_arm });
	animationPlayer.push_back({ modelPlayerR_arm_.get(), worldTransformPlayerR_arm });
	viewProjection_.Initialize();

	//debugCamera_ = new DebugCamera(1280, 720);

	//AxisIndicator::GetInstance()->SetVisible(true);
	//AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
	// player_ = new Player();
	player_ = std::make_unique<Player>();
	player_->Initialize(animationPlayer);
	player_->InitializeFloatingGimmick();

	modelWepon_.reset(Model::CreateFromOBJ("hammer"));
	player_->SetWepon(modelWepon_.get());

	// 天球
	modelSkydome_ = Model::CreateFromOBJ("skydome");
	skydome_.reset(new Skydome);
	skydome_->Initialize(modelSkydome_, Vector3(0.0f, 0.0f, 0.0f));

	// 地面
	modelGround_ = Model::CreateFromOBJ("Ground");
	ground_.reset(new Ground);
	ground_->Initialize(modelGround_, Vector3(0.0f, 0.0f, 0.0f));

	// カメラ生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(player_->GetWorldTransform());

	player_->SetViewProjection(&followCamera_->GetViewProjection());

	// 敵
	modelEnemyBody_.reset(Model::CreateFromOBJ("Enemy_Body"));
	modelEnemyWheel_.reset(Model::CreateFromOBJ("Enemy_Wheel"));
	std::vector<HierarchicalAnimation> animationEnemy;
	WorldTransform worldTransformEnemyBody = { .translation_{0.0f,0.5f,0.0f} };
	WorldTransform worldTransformEnemyWheel = { .translation_{0.0f, -0.2f, 0.0f} };
	animationEnemy.push_back({ modelEnemyBody_.get(), worldTransformEnemyBody });
	animationEnemy.push_back({ modelEnemyWheel_.get(), worldTransformEnemyWheel });

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(animationEnemy);
}

void GameScene::Update() {
	/*XINPUT_STATE joyState;
	Input::GetJoystickState(0,joyState);

	worldTransformObj_.translation_.x += float(joyState.Gamepad.sThumbLX) / SHRT_MAX;

	ImGui::Begin("obj1");
	ImGui::DragFloat3("Scale", &worldTransformObj_.scale_.x, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat3("Rotate", &worldTransformObj_.rotation_.x, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat3("Translate", &worldTransformObj_.translation_.x, 1.0f,-1000.0f, 1000.0f);
	ImGui::End();
	ImGui::Begin("obj2");
	ImGui::DragFloat3("Scale", &worldTransformObj2_.scale_.x, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat3("Rotate", &worldTransformObj2_.rotation_.x, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat3("Translate", &worldTransformObj2_.translation_.x, 1.0f, - 1000.0f, 1000.0f); ImGui::End();


	worldTransformObj_.UpdateMatrix();
	worldTransformObj2_.UpdateMatrix();

	viewProjection_.UpdateMatrix();*/
	player_->Update();
	enemy_->Update();
	//debugCamera_->Update();
	followCamera_->Update();

	if (isDebugCameraActive_) {
		//viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		//viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		//viewProjection_.TransferMatrix();
	}
	else {
		// viewProjection_.UpdateMatrix();
		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		//viewProjection_.TransferMatrix();
	}
}

void GameScene::Draw2D() {

}

void GameScene::Draw3D() {
/*	Primitive3D::PreDraw(dxCommon_->GetCommandList());
	Primitive3D::PostDraw();*/
	Model::PreDraw(dxCommon_->GetCommandList());
	skydome_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);
	player_->Draw(viewProjection_);
	enemy_->Draw(viewProjection_);

	Model::PostDraw();
}

