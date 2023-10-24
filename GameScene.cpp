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

	//床
	//flooar_.reset(new MovingFlooar);
	//flooar_->Initialize();
	/*
	for (int index = 0; index < 5;index++) {
		flooars_[index].reset(new MovingFlooar);
		flooars_[index]->Initialize();
	}*/
	flooars_[0].reset(new Flooar);
	flooars_[0]->Initialize();
	flooars_[1].reset(new MovingFlooar);
	flooars_[1]->Initialize();
	flooars_[1]->SetOffset({0.0f,0.0f,6.0f});
	flooars_[2].reset(new Flooar);
	flooars_[2]->Initialize();
	flooars_[2]->SetOffset({ 0.0f,0.0f,20.0f });
	flooars_[2]->SetSize({10.0f,0.0f,10.0f});
	flooars_[3].reset(new MovingFlooar);
	flooars_[3]->Initialize();
	flooars_[3]->SetOffset({ 0.0f,0.0f,34.0f });
	flooars_[4].reset(new Flooar);
	flooars_[4]->Initialize();
	flooars_[4]->SetOffset({ 0.0f,0.0f,40.0f });

	//ゴール
	goal_.reset(new Goal);
	goal_->Initialize();

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
	
	for (int index = 0; index < 5; index++) {
		flooars_[index]->Update();
	}
	//flooar_->Update();
	player_->Update();
	enemy_->Update();
	goal_->Update();
	//debugCamera_->Update();
	followCamera_->Update();
	//flooar_->Update();
	/*
	if (IsCollision(player_->GetOBB(),flooar_->GetOBB()))
	{
		player_->OnCollision(flooar_->GetWorldTransform());
	}
	else {
		player_->OutCollision();
	}*/
	bool isCollision = false;
	for (int index = 0; index < 5; index++) {
		if (IsCollision(player_->GetOBB(), flooars_[index]->GetOBB()))
		{
			player_->OnCollision(flooars_[index]->GetWorldTransform());
			isCollision = true;
		}
	}
	if (!isCollision) {
		player_->OutCollision();
	}
	if (IsCollision(player_->GetOBB(),goal_->GetOBB())) {
		player_->ReStart();
	}
	if (IsCollision(player_->GetOBB(), enemy_->GetSphere())) {
		player_->OnCollisionEnemy();
	}

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
	//ground_->Draw(viewProjection_);
	//flooar_->Draw(viewProjection_);
	for (int index = 0; index < 5; index++) {
		flooars_[index]->Draw(viewProjection_);
	}
	player_->Draw(viewProjection_);
	enemy_->Draw(viewProjection_);
	goal_->Draw(viewProjection_);
	Model::PostDraw();
}

