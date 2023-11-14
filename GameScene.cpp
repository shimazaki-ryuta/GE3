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

	particle.reset(Particle::Create(500));

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
	debugCamera_->SetRotate({ std::numbers::pi_v<float> / 3.0f,std::numbers::pi_v<float> ,0.0f });
	debugCamera_->SetPosition({0.0f, 23.0f, 10.0f});
#endif // _DEBUG
	usebillboard = true;
	emitter.count = 3;
	emitter.frequency = 0.5f;
	emitter.frequencyTime = 0.0f;
	emitter.transform.translate = {0,0,0};
	/*for (int index = 0; index < 10000; index++) {
		particle->MakeNewParticle(emitter.transform.translate);
	}*/
	accelerationField.acceleration = {15.0f,0.0f,0.0f};
	accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField.area.max = { 1.0f,1.0f,1.0f };
}

void GameScene::Update() {
#ifdef _DEBUG
	//デバッグカメラを有効化
	if (Input::GetKeyDown(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
		debugCamera_->SetUses(isDebugCameraActive_);
	}
	ImGui::Begin("FPS");
	ImGui::Text("%f", DeltaTime::GetFramePerSecond());
	ImGui::Text("%f", ImGui::GetIO().Framerate);
	ImGui::End();

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
	ImGui::Begin("Particle");
	ImGui::Checkbox("UseBillboard", &usebillboard);
	ImGui::DragFloat3("EmitterTranslate",&emitter.transform.translate.x,0.01f,-100.0f,100.0f);
	if (ImGui::Button("AddParticle")) {
		particle->Emit(emitter);
	}
	ImGui::End();
	particle->UseBillboard(usebillboard);
#endif // _DEBUG
	
	float deltaTime = 1.0f / 60.0f;
	emitter.frequencyTime += deltaTime;
	if (emitter.frequency <= emitter.frequencyTime) {
		particle->Emit(emitter);
		emitter.frequencyTime -= emitter.frequency;
	}
	std::vector<Particle::ParticleData>* particleData = particle->GetParticleDate();
	for (std::vector<Particle::ParticleData>::iterator particleIterator = particleData->begin(); particleIterator != particleData->end();particleIterator++) {
		if (IsCollision(accelerationField.area, (*particleIterator).transform.translate)) {
			(*particleIterator).velocity += deltaTime * accelerationField.acceleration;
		}
	}
		particle->Updade();

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
	//sprite_->Draw();
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
	particle->Draw(viewProjection_);
	Particle::PostDraw();
}

