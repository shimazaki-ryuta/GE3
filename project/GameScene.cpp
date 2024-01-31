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
GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;
	input_ = Input::GetInstance();

	audioHandle_ = AudioManager::GetInstance()->LoadWave("Alarm01.wav");
	//AudioManager::GetInstance()->PlayWave(audioHandle_);
#ifdef _DEBUG
	
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

	//DirectionalLight用のリソース
	directinalLightResource = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(), sizeof(DirectionalLight));
	directinalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directinalLightData));
	directinalLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	directinalLightData->direction = { 0.6f,-1.0f,0.0f };
	directinalLightData->direction = Normalize(directinalLightData->direction);
	directinalLightData->intensity = 0.6f;

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

	modelPlayerBody2_.reset(Model::CreateFromOBJ("float_Body"));
	modelPlayerHead2_.reset(Model::CreateFromOBJ("float_Head"));
	modelPlayerL_arm2_.reset(Model::CreateFromOBJ("float_L_arm"));
	modelPlayerR_arm2_.reset(Model::CreateFromOBJ("float_R_arm"));
	// std::vector<Model*> modelPlayers_;
	std::vector<HierarchicalAnimation> animationPlayer2;
	animationPlayer2.push_back({ modelPlayerBody2_.get(), worldTransformPlayerBody });
	animationPlayer2.push_back({ modelPlayerHead2_.get(), worldTransformPlayerHead });
	animationPlayer2.push_back({ modelPlayerL_arm2_.get(), worldTransformPlayerL_arm });
	animationPlayer2.push_back({ modelPlayerR_arm2_.get(), worldTransformPlayerR_arm });


	viewProjection_.Initialize();
	viewProjection_.rotation_.x = 1.0f;
	viewProjection_.UpdateMatrix();
	//debugCamera_ = new DebugCamera(1280, 720);
	toonShadowTextureHandle_ = TextureManager::LoadTexture("toonShadow1.png");
	player_ = std::make_unique<Player>();
	player_->Initialize(animationPlayer);
	player_->InitializeFloatingGimmick();
	player_->SetShadowTexture(toonShadowTextureHandle_);
	player_->SetOutLineData(0.05f, {0,0,1.0f,1.0f});
	modelWepon_.reset(Model::CreateFromOBJ("hammer"));
	//player_->SetWepon(modelWepon_.get());

	player2_ = std::make_unique<Player2>();
	player2_->Initialize(animationPlayer2);
	player2_->InitializeFloatingGimmick();
	player2_->SetShadowTexture(toonShadowTextureHandle_);
	player2_->SetOutLineData(0.05f, { 1.0f,0,0.0f,1.0f });
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
	//lockOn_->Update(player2_.get(), viewProjection_);

	particle.reset(Particle::Create(800));
	particle->UseBillboard(true);
	player_->SetParticle(particle.get());
	player2_->SetParticle(particle.get());

	ai_.reset(new PlayerAI);
	ai_->Initialize();
	ai_->SetPlayer1(player_.get());
	ai_->SetPlayer2(player2_.get());
	player_->SetFire(std::bind(&PlayerAI::Fire, ai_.get()));

	backTextureHandle_ = TextureManager::LoadTexture("back.png");
	backSprite_.reset(Sprite::Create(backTextureHandle_, { 0.0f,0.0f }, { 1280.0f,720.0f }, { 1.0f,1.0f,1.0f,1.0f }));
	backSprite_->SetAnchorPoint({ 0.0f,0.0f });
	backSprite_->SetBlendMode(Sprite::BlendMode::Normal);
	isEnd_ = false;
	isIngame_ = false;

	player_->ReStart();
	player2_->ReStart();
	Input::GetInstance()->GetJoystickState(0, preJoyState_);

	pressATextureHandle_ = TextureManager::LoadTexture("pressa.png");
	pressASprite_.reset(Sprite::Create(pressATextureHandle_, { 640.0f,600.0f }, { 128.0f * 2.0f,32.0f * 2.0f }, { 1.0f,1.0f,1.0f,1.0f }));

	endCount_ = 0;

	endTextureHandle_[0] = TextureManager::LoadTexture("win.png");
	endTextureHandle_[1] = TextureManager::LoadTexture("lose.png");
	endSprite_.reset(Sprite::Create(endTextureHandle_[0], { 640.0f,300.0f }, { 128.0f * 3.0f,32.0f * 3.0f }, { 1.0f,1.0f,1.0f,1.0f }));

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

	fadeSprite_.reset(Sprite::Create(TextureManager::LoadTexture("white2x2.png"), { 0.0f,0.0f }, { 1280.0f,720.0f }, { 0.0f,0.0f,0.0f,0.5f }));
	fadeSprite_->SetAnchorPoint({ 0.0f,0.0f });
	fadeSprite_->SetColor({0,0,0,0.0f});
	fadeSprite_->SetBlendMode(Sprite::BlendMode::Normal);
	isTransitionFade_ = false;
	isStart_ = false;
	fadeAlpha_ = 1.0f;

	lockOn_->Update(player2_.get(), viewProjection_);
	CollisionManager::GetInstance();
	colorPhase_ = 0;
	color_ = 0.0f;
}

void GameScene::Update() {
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	for (int index = 0; index < 1; index++) {
		flooars_[index]->Update();
	}

	switch (colorPhase_)
	{
	case 0:
		//directinalLightData->color = Vector4{ 1.0f-color_,color_, 0.0f, 1.0f };
		break;
	case 1:
		//directinalLightData->color = Vector4{0,1.0f-color_, color_, 1.0f };
		break;
	case 2:
		//directinalLightData->color = Vector4{color_,0, 1.0f-color_, 1.0f };
		break;
	default:
		break;
	}

	color_+=0.01f;
	if (color_>1.0f) {
		color_ = 0;
		colorPhase_++;
		if (colorPhase_>2) {
			colorPhase_ = 0;
		}
	}

	if (!isIngame_) {
		Vector3 offset = { 0.0f,8.0f,-20.0f };
		viewProjection_.translation_ = Transform(offset, MakeRotateMatrix(viewProjection_.rotation_));
		viewProjection_.rotation_.y += 0.01f;
		viewProjection_.rotation_.x = 0.10f;
		viewProjection_.UpdateMatrix();
		//viewProjection_.matView = followCamera_->GetViewProjection().matView;
		//viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		//viewProjection_.translation_ = followCamera_->GetViewProjection().translation_;
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			//isIngame_ = true;
			isStart_ = true;
			isTransitionFade_ = false;
		}
		if (!isStart_) {
			fadeAlpha_ -= 0.05f;
			if (fadeAlpha_ <0) {
				fadeAlpha_ = 0;
			}
		}
		else {
			fadeAlpha_ += 0.05f;
			if (fadeAlpha_ > 1.0f) {
				fadeAlpha_ = 1.0f;
				isIngame_ = true;
			}
		}

	}
	else {
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
			float alpha = float(endCount_) / 120.0f;
			endSprite_->SetColor({ 1.0f,1.0f,1.0f,alpha });
			endCount_++;
			if (endCount_ > 120) {
				endCount_ = 120;
			}
			if (endCount_ == 120) {
				if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
					isTransitionFade_=true;
					/*isIngame_ = false;
					player_->ReStart();
					player2_->ReStart();
					followCamera_->Reset();
					followCamera_->SetTarget(player_->GetWorldTransform());
					ai_->Initialize();
					endCount_ = 0;
					isEnd_ = false;*/
				}
			}
		}


		CollisionManager::GetInstance()->CheckAllCollisions();

		//PointLight
		for (uint32_t index = 0; index < pointLightMax_; ++index) {
			pointLightData[index].color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
			pointLightData[index].position = { 0.0f,1.0f,0.0f };
			pointLightData[index].intensity = 0.0f;
			pointLightData[index].radius = 10.0f;
			pointLightData[index].decay = 0.0f;
			pointLightData[index].isUse_ = 0;
		}
		uint32_t count=0;
		for (std::list<std::unique_ptr<Bullet>>::iterator ite = player_->GetBulletList().begin(); ite != player_->GetBulletList().end(); ite++) {
			if (count<pointLightMax_) {
				pointLightData[count].color = Vector4{ 1.0f, 0.6f, 0.2f, 1.0f };
				pointLightData[count].position = (*ite)->GetSphere().center;
				pointLightData[count].intensity = 2.0f;
				pointLightData[count].radius = 10.0f;
				pointLightData[count].decay = 0.8f;
				pointLightData[count].isUse_ = 1;
				count++;
			}
		}
		for (std::list<std::unique_ptr<Bullet>>::iterator ite = player2_->GetBulletList().begin(); ite != player2_->GetBulletList().end(); ite++) {
			if (count < pointLightMax_) {
				pointLightData[count].color = Vector4{ 1.0f, 0.6f, 0.2f, 1.0f };
				pointLightData[count].position = (*ite)->GetSphere().center;
				pointLightData[count].intensity = 2.0f;
				pointLightData[count].radius = 10.0f;
				pointLightData[count].decay = 0.8f;
				pointLightData[count].isUse_ = 1;
				count++;
			}
		}


		{
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
		if (!isTransitionFade_) {
			fadeAlpha_ -= 0.05f;
			if (fadeAlpha_ < 0) {
				fadeAlpha_ = 0;
			}
		}
		else {
			fadeAlpha_ += 0.05f;
			if (fadeAlpha_ > 1.0f) {
				for (uint32_t index = 0; index < pointLightMax_; ++index) {
					pointLightData[index].color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
					pointLightData[index].position = { 0.0f,1.0f,0.0f };
					pointLightData[index].intensity = 0.0f;
					pointLightData[index].radius = 10.0f;
					pointLightData[index].decay = 0.0f;
					pointLightData[index].isUse_ = 0;
				}
				fadeAlpha_ = 1.0f;
				//isIngame_ = true;
				isIngame_ = false;
				isStart_ = false;
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
	particle->Updade();
	preJoyState_ = joyState;
	buttonCount_++;
	if (buttonCount_ > 30) {
		buttonCount_ = 0;
		isButtonDraw_ = !isButtonDraw_;
	}
	fadeSprite_->SetColor({0,0,0,fadeAlpha_});
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
		if (endCount_ == 120) {
			if (isButtonDraw_) {
				pressASprite_->Draw();
			}
		}
	}
	fadeSprite_->Draw();
}

void GameScene::Draw3D() {
	//Primitive3D::PreDraw(dxCommon_->GetCommandList());
	//Primitive3D::PostDraw();
	Model::PreDraw(dxCommon_->GetCommandList());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(5, srvHandleGPU);
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());
	//Model::PreDraw(dxCommon_->GetCommandList());
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	skydome_->Draw(viewProjection_);
	for (int index = 0; index < 1; index++) {
		flooars_[index]->Draw(viewProjection_);
	}
	player_->Draw(viewProjection_);
	player2_->Draw(viewProjection_);

	Model::PostDraw();
	Model::PreDrawOutLine(dxCommon_->GetCommandList());
	player_->DrawOutLine(viewProjection_);
	player2_->DrawOutLine(viewProjection_);
	Model::PostDraw();
	if (isIngame_) {
		Particle::PreDraw(dxCommon_->GetCommandList());
		particle->Draw(viewProjection_);
		Particle::PostDraw();
	}
}

void GameScene::TransitionFade() {

}