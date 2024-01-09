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
#include "CollisionManager.h"
#include "RandomEngine.h"
GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;
	input_ = Input::GetInstance();

	CollisionManager::GetInstance()->ClearList();
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
	//player_->SetWepon(modelWepon_.get());

	player2_ = std::make_unique<Player2>();
	player2_->Initialize(animationPlayer);
	player2_->InitializeFloatingGimmick();
	//player2_->SetWepon(modelWepon_.get());

	modelBullet_.reset(Model::CreateFromOBJ("bullet"));
	//modelBullet_->SetShiniess(40.0f);
	modelBullet_->SetEnableLighting(0);
	player_->SetModelBullet(modelBullet_.get());
	player2_->SetModelBullet(modelBullet_.get());

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
	flooars_[0]->SetOffset({ 0.0f,0.0f,0.0f });
	flooars_[0]->SetSize({ 120.0f,0.0f,120.0f });

	
	// カメラ生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(player_->GetWorldTransform());
	followCamera_->Reset();

	player_->SetViewProjection(&followCamera_->GetViewProjection());
	
	lockOn_.reset(new LockOn);
	lockOn_->Initialize();
	
	//DirectionalLight用のリソース
	directinalLightResource = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(), sizeof(DirectionalLight));
	directinalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directinalLightData));
	directinalLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	directinalLightData->direction = { 0.0f,-1.0f,0.0f };
	directinalLightData->intensity = 1.0f;

	particle.reset(Particle::Create(500));
	particle->UseBillboard(true);
	player_->SetParticle(particle.get());
	player2_->SetParticle(particle.get());

	ai_.reset(new PlayerAI);
	ai_->Initialize();
	ai_->SetPlayer1(player_.get());
	ai_->SetPlayer2(player2_.get());
	player_->SetFire(std::bind(&PlayerAI::Fire,ai_.get()));

	backTextureHandle_ = TextureManager::LoadTexture("back.png");
	backSprite_.reset(Sprite::Create(backTextureHandle_, { 0.0f,0.0f }, { 1280.0f,720.0f }, { 1.0f,1.0f,1.0f,1.0f }));
	backSprite_->SetAnchorPoint({0.0f,0.0f});
	backSprite_->SetBlendMode(Sprite::BlendMode::Normal);
	isEnd_ = false;
	isIngame_ = false;

	player_->ReStart();
	player2_->ReStart();
	Input::GetInstance()->GetJoystickState(0, preJoyState_);

	pressATextureHandle_ = TextureManager::LoadTexture("pressa.png");
	pressASprite_.reset(Sprite::Create(pressATextureHandle_, { 640.0f,600.0f }, { 128.0f*2.0f,32.0f*2.0f }, { 1.0f,1.0f,1.0f,1.0f }));
	
	endCount_ = 0;

	endTextureHandle_[0] = TextureManager::LoadTexture("win.png");
	endTextureHandle_[1] = TextureManager::LoadTexture("lose.png");
	endSprite_.reset(Sprite::Create(endTextureHandle_[0], {640.0f,300.0f}, {128.0f * 3.0f,32.0f * 3.0f}, {1.0f,1.0f,1.0f,1.0f}));

	buttonCount_ = 0;
	isButtonDraw_ = false;

	shotTextureHandle_ = TextureManager::LoadTexture("shot.png");
	dashTextureHandle_ = TextureManager::LoadTexture("dash.png");
	jumpTextureHandle_ = TextureManager::LoadTexture("jump.png");
	shotSprite_.reset(Sprite::Create(shotTextureHandle_, { 300.0f,600.0f }, { 256.0f * 1.0f,32.0f * 1.0f }, { 1.0f,1.0f,1.0f,1.0f }));
	dashSprite_.reset(Sprite::Create(dashTextureHandle_, { 640.0f,600.0f }, { 256.0f * 1.0f,32.0f * 1.0f }, { 1.0f,1.0f,1.0f,1.0f }));
	jumpSprite_.reset(Sprite::Create(jumpTextureHandle_, { 980.0f,600.0f }, { 256.0f * 1.0f,32.0f * 1.0f }, { 1.0f,1.0f,1.0f,1.0f }));
	titleTextureHandle_ = TextureManager::LoadTexture("title.png");
	titleSprite_.reset(Sprite::Create(titleTextureHandle_, { 640.0f,320.0f }, { 256.0f * 3.0f,32.0f * 3.0f }, { 1.0f,1.0f,1.0f,1.0f }));

}

void GameScene::Update() {

	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	for (int index = 0; index < 1; index++) {
		flooars_[index]->Update();
	}

	if (!isIngame_) {
		Vector3 offset = { 0.0f,8.0f,-20.0f };
		viewProjection_.translation_ = Transform(offset,MakeRotateMatrix(viewProjection_.rotation_));
		viewProjection_.rotation_.y += 0.01f;
		viewProjection_.rotation_.x = 0.10f;
		viewProjection_.UpdateMatrix();
		//viewProjection_.matView = followCamera_->GetViewProjection().matView;
		//viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		//viewProjection_.translation_ = followCamera_->GetViewProjection().translation_;
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			isIngame_ = true;
		}
	}
	else  {
		//flooar_->Update();
		player_->Update();
		if (player_->GetWorldTransform()->GetWorldPosition().y < -20.0f) {
			player_->ReStart();
			/*for (std::list<std::unique_ptr<Enemy>>::iterator enemy = enemies_.begin(); enemy != enemies_.end(); enemy++) {
				enemy->get()->ReStart();
			}*/
		}

		ai_->Update();

		player2_->SetData(ai_->GetData());
		player2_->Update();
		if (player2_->GetWorldTransform()->GetWorldPosition().y < -20.0f) {
			player2_->ReStart();

		}

		followCamera_->Update();

		bool isCollision = false;
		for (int index = 0; index < 1; index++) {
			if (IsCollision(player_->GetOBB(), flooars_[index]->GetOBB()))
			{
				player_->OnCollision(flooars_[index]->GetWorldTransform());
				isCollision = true;
			}
		}
		if (!isCollision) {
			player_->OutCollision();
		}

		isCollision = false;
		for (int index = 0; index < 1; index++) {
			if (IsCollision(player2_->GetOBB(), flooars_[index]->GetOBB()))
			{
				player2_->OnCollision(flooars_[index]->GetWorldTransform());
				isCollision = true;
			}
		}
		if (!isCollision) {
			player2_->OutCollision();
		}

		//クリア
		if (!isEnd_) {
			for (std::list<std::unique_ptr<Bullet>>::iterator ite = player_->GetBulletList().begin(); ite != player_->GetBulletList().end(); ite++) {
				if (IsCollision(player2_->GetOBB(), (*ite)->GetSphere()) && !(*ite)->GetIsDead()) {
					Particle::ParticleData particleData;
					for (uint32_t count = 0; count < 20; count++) {
						particleData.transform.scale = { 0.5f,0.5f,0.5f };
						particleData.transform.rotate = { 0.0f,0.0f,0.0f };
						particleData.transform.translate = player2_->GetWorldTransformBody()->GetWorldPosition();
						particleData.velocity = { RandomEngine::GetRandom(-1.0f,1.0f),RandomEngine::GetRandom(-1.0f,1.0f), RandomEngine::GetRandom(-1.0f,1.0f) };
						particleData.color = { RandomEngine::GetRandom(0.0f,1.0f),0,0,1.0f };
						particleData.lifeTime = RandomEngine::GetRandom(1.0f, 3.0f);
						particleData.currentTime = 0;
						particle->MakeNewParticle(particleData);
					}
					endSprite_->SetTextureHandle(endTextureHandle_[0]);
					player2_->SetIsDead(true);
					followCamera_->Shake();
					followCamera_->SetTarget(player2_->GetWorldTransformBody());
					isEnd_ = true;
				}
			}
		}
		
		//ゲームオーバー
		if (!isEnd_) {
			for (std::list<std::unique_ptr<Bullet>>::iterator ite = player2_->GetBulletList().begin(); ite != player2_->GetBulletList().end(); ite++) {
				if (IsCollision(player_->GetOBB(), (*ite)->GetSphere()) && !(*ite)->GetIsDead()) {
					Particle::ParticleData particleData;
					for (uint32_t count = 0; count < 20; count++) {
						particleData.transform.scale = { 0.5f,0.5f,0.5f };
						particleData.transform.rotate = { 0.0f,0.0f,0.0f };
						particleData.transform.translate = player_->GetWorldTransform()->GetWorldPosition();
						particleData.transform.translate.y += 1.5f;
						particleData.velocity = { RandomEngine::GetRandom(-1.0f,1.0f),RandomEngine::GetRandom(-1.0f,1.0f), RandomEngine::GetRandom(-1.0f,1.0f) };
						particleData.color = { RandomEngine::GetRandom(0.0f,1.0f),0,0,1.0f };
						particleData.lifeTime = RandomEngine::GetRandom(1.0f, 3.0f);
						particleData.currentTime = 0;
						particle->MakeNewParticle(particleData);
					}
					endSprite_->SetTextureHandle(endTextureHandle_[1]);
					player_->SetIsDead(true);
					followCamera_->Shake();
					//followCamera_->SetTarget(player2_->GetWorldTransformBody());
					isEnd_ = true;
				}
			}
		}
		if (isEnd_) {
			float alpha = float(endCount_) / 180.0f;
			endSprite_->SetColor({1.0f,1.0f,1.0f,alpha});
			endCount_++;
			if (endCount_>180) {
				endCount_ = 180;
			}
			if (endCount_ == 180) {
				if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
					isIngame_ = false;
					player_->ReStart();
					player2_->ReStart();
					followCamera_->Reset();
					followCamera_->SetTarget(player_->GetWorldTransform());
					ai_->Initialize();
					endCount_ = 0;
					isEnd_ = false;
				}
			}
		}


		CollisionManager::GetInstance()->CheckAllCollisions();

		if (isDebugCameraActive_) {
			//viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			//viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			//viewProjection_.TransferMatrix();
		}
		else {
			// viewProjection_.UpdateMatrix();
			viewProjection_.matView = followCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
			viewProjection_.translation_ = followCamera_->GetViewProjection().translation_;
			//viewProjection_.TransferMatrix();
		}
		lockOn_->Update(player2_.get(), viewProjection_);
		followCamera_->SetLockOnTarget(nullptr);
		player_->SetTarget(lockOn_->GetTarget());
		player2_->SetTarget(player_->GetWorldTransform());
		
	}
	particle->Updade();
	preJoyState_ = joyState;
	buttonCount_++;
	if (buttonCount_>30){
		buttonCount_ = 0;
		isButtonDraw_ = !isButtonDraw_;
	}
}

void GameScene::Draw2D() {
	if (!isIngame_) {
		titleSprite_->Draw();
		if (isButtonDraw_) {
			pressASprite_->Draw();
		}
	}
	if (isIngame_ && !isEnd_) {
		shotSprite_->Draw();
		dashSprite_->Draw();
		jumpSprite_->Draw();

		lockOn_->Draw();
	}
	if (isEnd_) {
		backSprite_->Draw();
		endSprite_->Draw();
		if (endCount_ == 180) {
			if (isButtonDraw_) {
				pressASprite_->Draw();
			}
		}
	}
}

void GameScene::Draw3D() {
	Model::PreDraw(dxCommon_->GetCommandList());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	skydome_->Draw(viewProjection_);
	for (int index = 0; index < 1; index++) {
		flooars_[index]->Draw(viewProjection_);
	}
	player_->Draw(viewProjection_);
	player2_->Draw(viewProjection_);
	
	Model::PostDraw();
	if (isIngame_) {
		Particle::PreDraw(dxCommon_->GetCommandList());
		particle->Draw(viewProjection_);
		Particle::PostDraw();
	}
}

