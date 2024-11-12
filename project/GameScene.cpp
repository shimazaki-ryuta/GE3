#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <list>
#include <fstream>
//#include  "Primitive3D.h"
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
#include "CommonFiles/SRVManager.h"
GameScene::GameScene() {

}

GameScene::~GameScene() {
#ifdef DEMO
	sceneLoader_->EndReceveThread();
#endif // _DEBUG
}

void GameScene::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;
	input_ = Input::GetInstance();

	debugCamera_.reset(new DebugCamera);
	debugCamera_->Initialize();

	//audioHandle_ = AudioManager::GetInstance()->LoadWave("Alarm01.wav");
	audioHandle_ = AudioManager::GetInstance()->LoadAudio("1.mp3");
	//AudioManager::GetInstance()->PlayWave(audioHandle_);


	sceneLoader_.reset(new SceneLoader);
	sceneLoader_->LoadFile("testScene");
	modelList_.clear();
	sceneLoader_->CreateModelList(modelList_);
	objects_.clear();
	sceneLoader_->CreateObjects(objects_);
	sceneLoader_->CreateTerrain(terrain_);
	sceneLoader_->ReadTerrainVerticesFromFile(terrain_,"PlayerHead");
#ifdef DEMO

	sceneLoader_->StartReceveJson();

	isDebugCameraActive_ = false;
	debugCamera_->SetUses(isDebugCameraActive_);
	//debugCamera_->SetRotate({ std::numbers::pi_v<float> / 3.0f,std::numbers::pi_v<float> ,0.0f });
	debugCamera_->SetPosition({ 0.0f, 1.7f, -10.0f });
#endif // _DEBUG

	CreateLight();

	//skybox
	skyBox_.reset(new SkyBox);
	skyBox_->Initialize(TextureManager::LoadTexture("skyBox.dds"));
	skyBox_->SetColor({0.25f,0.25f,0.25f,1.0f});
	worldTransformSkyBox_.Initialize();
	worldTransformSkyBox_.scale_ = { 1000.0f,1000.0f,1000.0f };
	worldTransformSkyBox_.UpdateMatrix();

	// 地面
	modelGround_ = new Model();
	modelGround_->Create("Resources/human", "walk.gltf");

	ground_.reset(new Ground);
	ground_->Initialize(modelGround_, Vector3(0.0f, 0.0f, 0.0f));
	ground_->SetPerspectiveTextureHandle(skyBox_->GetTextureHandle());

	CollisionManager::GetInstance()->ClearList();

	CollisionManager::GetInstance()->ClearList();
	// 3Dモデルデータの生成
	//model_.reset(Model::CreateFromOBJ("Player", true));
	modelPlayerBody_.reset(Model::CreateFromOBJ("float_Body"));
	modelPlayerBody_->SetPerspectivTextureHandle(skyBox_->GetTextureHandle());
	Model* modelpl1 = new Model;
	modelpl1->Create("Resources/Player", "player.gltf");
	modelpl1->SetPerspectivTextureHandle(skyBox_->GetTextureHandle());
	modelPlayerHead_.reset(modelpl1);
	//modelPlayerHead_.reset(Model::CreateFromOBJ("float_Head"));
	modelPlayerL_arm_.reset(Model::CreateFromOBJ("float_L_arm"));
	modelPlayerL_arm_->SetPerspectivTextureHandle(skyBox_->GetTextureHandle());
	modelPlayerR_arm_.reset(Model::CreateFromOBJ("float_R_arm"));
	modelPlayerR_arm_->SetPerspectivTextureHandle(skyBox_->GetTextureHandle());
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
	modelPlayerBody2_->SetPerspectivTextureHandle(skyBox_->GetTextureHandle());
	Model* modelpl2 = new Model;
	modelpl2->Create("Resources/Player", "player.gltf");
	modelpl2->SetPerspectivTextureHandle(skyBox_->GetTextureHandle());
	modelPlayerHead2_.reset(modelpl2);
	//modelPlayerHead2_.reset(Model::CreateFromOBJ("float_Head"));
	modelPlayerL_arm2_.reset(Model::CreateFromOBJ("float_L_arm"));
	modelPlayerL_arm2_->SetPerspectivTextureHandle(skyBox_->GetTextureHandle());
	modelPlayerR_arm2_.reset(Model::CreateFromOBJ("float_R_arm"));
	modelPlayerR_arm2_->SetPerspectivTextureHandle(skyBox_->GetTextureHandle());
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
	player_->SetOutLineData(0.01f, { 0,0,1.0f,1.0f });
	//player_->SetWepon(modelWepon_.get());

	player2_ = std::make_unique<Player2>();
	player2_->Initialize(animationPlayer2);
	player2_->InitializeFloatingGimmick();
	player2_->SetShadowTexture(toonShadowTextureHandle_);
	player2_->SetOutLineData(0.01f, { 1.0f,0,0.0f,1.0f });
	//player2_->SetWepon(modelWepon_.get());
	Model* model = new Model;
	model->Create("Resources/bullet", "bullet.gltf");
	modelBullet_.reset(model);
	bulletAnimation_.reset(new Animation);
	bulletAnimation_->Initialize();
	bulletAnimation_->LoadAnimationFile("Resources/bullet", "bullet.gltf");
	//bulletAnimation_->SetPlaySpeed(5.0f);
	//modelBullet_.reset(Model::CreateFromOBJ("bullet"));
	//modelBullet_->SetShiniess(40.0f);
	modelBullet_->SetEnableLighting(0);
	modelBullet_->SetGrowStrength(1.0f);
	player_->SetModelBullet(modelBullet_.get());
	player2_->SetModelBullet(modelBullet_.get());
	player_->SetBulletAnimation(bulletAnimation_.get());
	player2_->SetBulletAnimation(bulletAnimation_.get());
	// 天球
	//modelSkydome_ = Model::CreateFromOBJ("skydome");
	//skydome_.reset(new Skydome);
	//skydome_->Initialize(modelSkydome_, Vector3(0.0f, 0.0f, 0.0f));

	// 地面
	modelGround_ = new Model();
	modelGround_->Create("Resources/human", "walk.gltf");
	//modelGround_->SetGrowStrength(1.0f);

	//modelPlayerHead_.reset(model);
	ground_.reset(new Ground);
	ground_->Initialize(modelGround_, Vector3(0.0f, 0.0f, 0.0f));
	ground_->SetPerspectiveTextureHandle(skyBox_->GetTextureHandle());

	//床
	flooars_[0].reset(new Flooar);
	flooars_[0]->Initialize();
	flooars_[0]->SetOffset({ 0.0f,0.0f,0.0f });
	flooars_[0]->SetSize({ 120.0f,0.0f,120.0f });
	flooars_[0]->SetPerspectiveTextureHandle(skyBox_->GetTextureHandle());

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

	endCount_ = 0;

	endTextureHandle_[0] = TextureManager::LoadTexture("win.png");
	endTextureHandle_[1] = TextureManager::LoadTexture("lose.png");
	endSprite_.reset(Sprite::Create(endTextureHandle_[0], { 640.0f,300.0f }, { 128.0f * 3.0f,32.0f * 3.0f }, { 1.0f,1.0f,1.0f,1.0f }));

	buttonCount_ = 0;
	isButtonDraw_ = false;

	fadeSprite_.reset(Sprite::Create(TextureManager::LoadTexture("white2x2.png"), { 0.0f,0.0f }, { 1280.0f,720.0f }, { 0.0f,0.0f,0.0f,0.5f }));
	fadeSprite_->SetAnchorPoint({ 0.0f,0.0f });
	fadeSprite_->SetColor({ 0,0,0,0.0f });
	fadeSprite_->SetBlendMode(Sprite::BlendMode::Normal);
	isTransitionFade_ = false;
	isStart_ = false;
	fadeAlpha_ = 1.0f;

	lockOn_->Update(player2_.get(), viewProjection_);

	CollisionManager::GetInstance();
	colorPhase_ = 0;
	color_ = 0.0f;

	grayScaleValue_ = 0.0f;
	dxCommon_->SetGraiScaleStrength(grayScaleValue_);

	viewProjection_.Initialize();
	hsvFilter_ = {0.0f,0.0f,0.0f,0.0f};

	state_ = std::bind(&GameScene::Idle, this);
}

void GameScene::Update() {
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);
	FromBlenderUpdate();
#ifdef DEMO

	if (Input::GetKeyDown(DIK_RSHIFT)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
		debugCamera_->SetUses(isDebugCameraActive_);
	}
	if (isDebugCameraActive_) {
		debugCamera_->Update();
	}
	ImGui::Begin("FPS");
	ImGui::Text("%f", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("HSV");
	ImGui::DragFloat("h",&hsvFilter_.x,0.01f);
	ImGui::DragFloat("s", &hsvFilter_.y, 0.01f);
	ImGui::DragFloat("v", &hsvFilter_.z, 0.01f);
	ImGui::End();

	ImGui::Begin("SkyBox");
	ImGui::ColorEdit3("color", &skyColor_.x);
	ImGui::DragFloat3("expornent", &skyExpornent_.x, 0.01f);
	ImGui::End();
	skyBox_->SetColor(skyColor_);
	skyBox_->SetExpornentiation(skyExpornent_);
	dxCommon_->SetHSVFilter(hsvFilter_);
#endif // _DEBUG

	for (std::unique_ptr<GameObject>& object : objects_) {
		object->Update();
	}
	terrain_->Update();

	for (int index = 0; index < 1; index++) {
		flooars_[index]->Update();
	}

	state_();

	particle->Updade();
	preJoyState_ = joyState;
	buttonCount_++;
	if (buttonCount_ > 30) {
		buttonCount_ = 0;
		isButtonDraw_ = !isButtonDraw_;
	}
	fadeSprite_->SetColor({ 0,0,0,fadeAlpha_ });
	dxCommon_->SetGraiScaleStrength(grayScaleValue_);

	followCamera_->Update();
	Fade();

	//デバッグカメラ
	if (isDebugCameraActive_) {
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
	}
}

void GameScene::CreateLight() {
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
		pointLightData[index].color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
		pointLightData[index].position = { 0.0f,1.0f,0.0f };
		pointLightData[index].intensity = 1.0f;
		pointLightData[index].radius = 10.0f;
		pointLightData[index].decay = 0.0f;
		pointLightData[index].isUse_ = 0;
	}

	uint32_t handle = SRVManager::GetInstance()->CreateSRV(pointLightResource.Get(), &srvDesc);
	srvHandleGPU = SRVManager::GetInstance()->GetGPUHandle(handle);

	directinalLightResource = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(DirectionalLight));
	directinalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directinalLightData));
	directinalLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	directinalLightData->direction = { 0.6f,-1.0f,0.0f };
	directinalLightData->direction = Normalize(directinalLightData->direction);
	directinalLightData->intensity = 0.6f;

	//PointLight用のリソース
	spotLightResource = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(SpotLight));
	spotLightResource->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));
	spotLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	spotLightData->position = { 2.0f,1.25f,0.0f };
	spotLightData->intensity = 0.0f;
	spotLightData->direction = Normalize(Vector3{ -1.0f,-1.0f,0.0f });
	spotLightData->distance = 7.0f;
	spotLightData->decay = 2.0f;
	spotLightData->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
}

void GameScene::FromBlenderUpdate() {
#ifdef DEMO
	sceneLoader_->CreateModelList(modelList_);
	sceneLoader_->ApplyTerrainTransform(terrain_);
	sceneLoader_->ApplyRecevedData(objects_);
	sceneLoader_->ApplyTerrainVertices(terrain_);
#endif // _DEBUG
}

void GameScene::Idle() {
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);
	grayScaleValue_ = 0.0f;
	Vector3 offset = { 0.0f,2.0f,-15.0f };
	viewProjection_.translation_ = Transform(offset, MakeRotateMatrix(viewProjection_.rotation_));
	viewProjection_.rotation_.y += 0.01f;
	viewProjection_.rotation_.x = 0.10f;
	viewProjection_.UpdateMatrix();
	if (((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) || input_->GetKeyDown(DIK_SPACE)) {
		isStart_ = true;
		isTransitionFade_ = false;
		state_ = std::bind(&GameScene::Play, this);
	}
	if (!isStart_) {
		fadeAlpha_ -= 0.05f;
		if (fadeAlpha_ < 0) {
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

void GameScene::Play() {
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);
	player_->Update();
	if (player_->GetWorldTransform()->GetWorldPosition().y < -20.0f) {
		player_->ReStart();
	}

	ai_->Update();

	player2_->SetData(ai_->GetData());
	player2_->Update();
	if (player2_->GetWorldTransform()->GetWorldPosition().y < -20.0f) {
		player2_->ReStart();

	}

	//objectのコライダーリスト取得
	std::list<Collider*> colliderList;
	colliderList.clear();
	for (std::unique_ptr<GameObject>& object : objects_) {
		object->AppendColliderList(colliderList);
	}

	bool isCollision = false;
	for (int index = 0; index < 1; index++) {
		if (IsCollision(player_->GetOBB(), flooars_[index]->GetOBB()))
		{
			player_->OnCollision(flooars_[index]->GetWorldTransform());
			isCollision = true;
		}
	}
	//objectとの当たり判定

	for (std::unique_ptr<GameObject>& object : objects_) {
		if (object->GetCollider()) {
			if (IsCollision(player_->GetOBB(), object->GetCollider()->GetColliderShape().sphere))
			{
				player_->OnCollisionSphere(*object->GetWorldTransform(), object->GetCollider()->GetColliderShape().sphere);
			}
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
				AudioManager::GetInstance()->PlayWave(audioHandle_);
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
				AudioManager::GetInstance()->PlayWave(audioHandle_);
				endSprite_->SetTextureHandle(endTextureHandle_[1]);
				player_->SetIsDead(true);
				followCamera_->Shake();
				isEnd_ = true;
			}
		}
	}
	if (isEnd_) {
		state_ = std::bind(&GameScene::End, this);
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
	uint32_t count = 0;
	for (std::list<std::unique_ptr<Bullet>>::iterator ite = player_->GetBulletList().begin(); ite != player_->GetBulletList().end(); ite++) {
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
		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		viewProjection_.translation_ = followCamera_->GetViewProjection().translation_;
	}
	lockOn_->Update(player2_.get(), viewProjection_);
	followCamera_->SetLockOnTarget(nullptr);
	player_->SetTarget(lockOn_->GetTarget());
	player2_->SetTarget(player_->GetWorldTransform());
}

void GameScene::End() {
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);
	player_->Update();
	if (player_->GetWorldTransform()->GetWorldPosition().y < -20.0f) {
		player_->ReStart();
	}

	ai_->Update();

	player2_->SetData(ai_->GetData());
	player2_->Update();
	if (player2_->GetWorldTransform()->GetWorldPosition().y < -20.0f) {
		player2_->ReStart();

	}
	//grayscale
	if (player_->GetIsDead()) {
		grayScaleValue_ += 0.01f;
		if (grayScaleValue_ > 1.0f) {
			grayScaleValue_ = 1.0f;
		}
	}
	float alpha = float(endCount_) / 120.0f;
	endSprite_->SetColor({ 1.0f,1.0f,1.0f,alpha });
	endCount_++;
	if (endCount_ > 120) {
		endCount_ = 120;
	}
	if (endCount_ == 120) {
		if (((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) || input_->GetKeyDown(DIK_SPACE)) {
			isTransitionFade_ = true;
		}
	}
}

void GameScene::Fade() {
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
			state_ = std::bind(&GameScene::Idle, this);
		}
	}
}

void GameScene::Draw2D() {

}

void GameScene::Draw3D() {
	
	Model::PreDraw(dxCommon_->GetCommandList());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(5, srvHandleGPU);
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());

	for (std::unique_ptr<GameObject>& object : objects_) {
		object->Draw(viewProjection_, modelList_);
	}
	terrain_->Draw(viewProjection_);

	for (int index = 0; index < 1; index++) {
		//flooars_[index]->Draw(viewProjection_);
	}
	player_->Draw(viewProjection_);
	player2_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);

	Model::PostDraw();
	Model::PreDrawOutLine(dxCommon_->GetCommandList());
	player_->DrawOutLine(viewProjection_);
	player2_->DrawOutLine(viewProjection_);
	ground_->DrawOutLine(viewProjection_);
	Model::PostDraw();


	//skybox描画
	skyBox_->Draw(worldTransformSkyBox_, viewProjection_);

	if (isIngame_ || 1) {
		Particle::PreDraw(dxCommon_->GetCommandList());
		particle->Draw(viewProjection_);
		Particle::PostDraw();
	}

}
