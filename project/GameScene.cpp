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
	debugCamera_.reset(new DebugCamera);
	debugCamera_->Initialize();
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
		pointLightData[index].intensity = 0.0f;
		pointLightData[index].radius = 10.0f;
		pointLightData[index].decay = 0.1f;
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
	directinalLightData->intensity = 0.2f;

	

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

	viewProjection_.Initialize();

	// 天球
	modelSkydome_ = Model::CreateFromOBJ("skydome");
	skydome_.reset(new Skydome);
	skydome_->Initialize(modelSkydome_, Vector3(0.0f, 0.0f, 0.0f));

	// 地面
	modelGround_ = Model::CreateFromOBJ("terrain");
	ground_.reset(new Ground);
	ground_->Initialize(modelGround_, Vector3(0.0f, 0.0f, 0.0f));

	uvCheckerTextureHandle_ = TextureManager::LoadTexture("uvChecker.png");
	monsterTextureHandle_ = TextureManager::LoadTexture("monsterBall.png");
	particle.reset(Particle::Create(500));
	particle->UseBillboard(true);
	for (int i = 0; i < 500; i++) {
		Particle::ParticleData pData = { 0 };
		pData.color = { RandomEngine::GetRandom(0.0f,1.0f),RandomEngine::GetRandom(0.0f,1.0f) ,RandomEngine::GetRandom(0.0f,1.0f) ,1.0f };
		pData.lifeTime = 3.0f;
		pData.transform = { {1.0f,1.0f,1.0f},0,0 };
		pData.velocity = { RandomEngine::GetRandom(-1.0f,1.0f),RandomEngine::GetRandom(-1.0f,1.0f) ,RandomEngine::GetRandom(-1.0f,1.0f) };
		particle->MakeNewParticle(pData);
	}

	sprite_.reset(Sprite::Create(uvCheckerTextureHandle_, { 0,0 }, { 720,360 }, { 1.0f,1.0f,1.0f,1.0f }));
	sprite_->SetAnchorPoint({ 0,0 });
	spritePosition_ = { 0,0 };
	ancorPoint_ = { 0,0 };
	rotate_ = 0;
	leftTop = { 0,0 };
	rightDown = { 720.0f,360.0f };

	audioHandle_ = AudioManager::GetInstance()->LoadWave("Alarm01.wav");
	AudioManager::GetInstance()->PlayWave(audioHandle_);
#ifdef _USEIMGUI
	isDebugCameraActive_ = true;
	debugCamera_->SetUses(isDebugCameraActive_);
	//debugCamera_->SetRotate({ std::numbers::pi_v<float> / 3.0f,std::numbers::pi_v<float> ,0.0f });
	//debugCamera_->SetPosition({ 0.0f, 1.3f, -17.0f });
	cameraTransform_ = { 0.0f, 1.3f, -17.0f };
	cameraRotate_ = {0.0f,0.0f,0.0f};
#endif // _DEBUG
	sphere_.reset(Model::CreateFromOBJ("uvSphere"));
	sphere_->SetEnableLighting(2);
	worldTransformSphere_.Initialize();
	//worldTransformSphere_.rotation_.y = 3.14f / 2.0f;
	worldTransformSphere_.translation_.y = 1.5f;
	shininess_ = 40.0f;
	lineColor_.w = 1.0f;
	lineWidth_ = { 0.05f,0.05f,0.05f };
	toonShadowTextureHandle_ = TextureManager::LoadTexture("toonShadow1.png");
	sphere_->SetToonShadowTextureHandle(toonShadowTextureHandle_);
	//sphere_->SetToonShadowTextureHandle(uvCheckerTextureHandle_);
}

void GameScene::Update() {
		//PointLight
	ImGui::Begin("PointLight"); 
		//pointLightData->color.x = RandomEngine::GetRandom(0.0f, 1.0f);
		//pointLightData->color.y = RandomEngine::GetRandom(0.0f, 1.0f);
		//pointLightData->color.z = RandomEngine::GetRandom(0.0f, 1.0f);

		for (uint32_t index = 0; index < 4; ++index) {
			char name[24];
			sprintf_s(name,24,"PointLight%d",index);
			ImGui::BeginGroup();
			sprintf_s(name, 24, "Position%d", index);
			ImGui::DragFloat3(name, &pointLightData[index].position.x);
			sprintf_s(name, 24, "Intensity%d", index);
			ImGui::SliderFloat(name, &pointLightData[index].intensity, 0.0f, 1.0f, 0);
			sprintf_s(name, 24, "Radius%d", index);
			ImGui::DragFloat(name, &pointLightData[index].radius);
			sprintf_s(name, 24, "Decay%d", index);
			ImGui::DragFloat(name, &pointLightData[index].decay, 0.1f);
			sprintf_s(name, 24, "Color%d", index);
			ImGui::ColorEdit4(name, &pointLightData[index].color.x);
			pointLightData[index].isUse_ = 1;
			ImGui::EndGroup();
			
			//ImGui::TreePop();
		}
		ImGui::End();
	

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
		ImGui::End();
		/*
		ImGui::Begin("PointLight");
		ImGui::DragFloat3("position", &pointLightData->position.x);
		ImGui::SliderFloat("Intensity", &pointLightData->intensity, 0.0f, 1.0f, 0);
		ImGui::DragFloat("radius", &pointLightData->radius);
		ImGui::DragFloat("decay", &pointLightData->decay, 0.1f);
		ImGui::ColorEdit4("Color", &pointLightData->color.x);
		ImGui::End();*/
		//pointLightData->color.x = RandomEngine::GetRandom(0.0f, 1.0f);
		//pointLightData->color.y = RandomEngine::GetRandom(0.0f, 1.0f);
		//pointLightData->color.z = RandomEngine::GetRandom(0.0f, 1.0f);

		ImGui::Begin("SpotLight");
		ImGui::DragFloat3("position", &spotLightData->position.x);
		ImGui::DragFloat3("direction", &spotLightData->direction.x);
		spotLightData->direction = Normalize(spotLightData->direction);
		ImGui::SliderFloat("Intensity", &spotLightData->intensity, 0.0f, 1.0f, 0);
		ImGui::DragFloat("distance", &spotLightData->distance);
		ImGui::DragFloat("decay", &spotLightData->decay, 0.1f);
		ImGui::DragFloat("Angle", &spotLightData->cosAngle, 0.1f);
		ImGui::ColorEdit4("Color", &spotLightData->color.x);
		ImGui::End();

		sphere_->SetShiniess(shininess_);
		ImGui::Begin("Sphere");
		ImGui::DragFloat3("scale", &worldTransformSphere_.scale_.x, 1.0f);
		ImGui::DragFloat3("rotate", &worldTransformSphere_.rotation_.x, 1.0f);
		ImGui::DragFloat3("position", &worldTransformSphere_.translation_.x, 1.0f);
		ImGui::DragFloat("Shininess", &shininess_, 1.0f, 0.0f, 200.0f);
		ImGui::DragFloat3("lineWidth", &lineWidth_.x, 0.001f);
		ImGui::SliderInt("ShadingType", &(shadeType_), 0, 1);
		ImGui::ColorEdit4("Color", &lineColor_.x);
		ImGui::End();
		sphere_->SetOutLineColor(lineColor_);
		sphere_->SetOutLineWidth(lineWidth_);
		sphere_->SetShadingType(shadeType_);
		worldTransformSphere_.UpdateMatrix();

		ImGui::Begin("Sprite");
		ImGui::DragFloat2("position", &spritePosition_.x, 1.0f);
		ImGui::DragFloat2("anchor", &ancorPoint_.x, 0.1f);
		ImGui::DragFloat("rotate", &rotate_, 0.1f);
		ImGui::DragFloat2("leftTop", &leftTop.x, 1.0f);
		ImGui::DragFloat2("rightDown", &rightDown.x, 1.0f);
		ImGui::Checkbox("FlipX",&isFlipX_);
		ImGui::Checkbox("FlipY", &isFlipY_);
		ImGui::End();
		sprite_->SetPosition(spritePosition_);
		sprite_->SetAnchorPoint(ancorPoint_);
		sprite_->SetRotate(rotate_);
		sprite_->SetRange(leftTop, rightDown);
		sprite_->SetIsFlipX(isFlipX_);
		sprite_->SetIsFlipY(isFlipY_);

		//sphere_->SetOutLineColor(Vector4{ RandomEngine::GetRandom(0,1.0f),RandomEngine::GetRandom(0,1.0f) ,RandomEngine::GetRandom(0,1.0f) ,1.0f });

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
		float speed=0.1f;
		if (input_->GetKey(DIK_W)) {
			cameraTransform_.z += speed;
		}
		if (input_->GetKey(DIK_S)) {
			cameraTransform_.z -= speed;
		}
		if (input_->GetKey(DIK_A)) {
			cameraTransform_.x -= speed;
		}
		if (input_->GetKey(DIK_D)) {
			cameraTransform_.x += speed;
		}
		if (input_->GetKey(DIK_E)) {
			cameraTransform_.y += speed;
		}
		if (input_->GetKey(DIK_Q)) {
			cameraTransform_.y -= speed;
		}
		if (input_->GetKey(DIK_LEFT)) {
			cameraRotate_.y -= 0.01f;
		}
		if (input_->GetKey(DIK_RIGHT)) {
			cameraRotate_.y += 0.01f;
		}
		if (input_->GetKey(DIK_UP)) {
			cameraRotate_.x -= 0.01f;
		}
		if (input_->GetKey(DIK_DOWN)) {
			cameraRotate_.x += 0.01f;
		}
		debugCamera_->SetPosition(cameraTransform_);
		debugCamera_->SetRotate(cameraRotate_);
		debugCamera_->Update();
			Particle::ParticleData pData = { 0 };
			pData.color = { RandomEngine::GetRandom(0.0f,1.0f),RandomEngine::GetRandom(0.0f,1.0f) ,RandomEngine::GetRandom(0.0f,1.0f) ,1.0f };
			pData.lifeTime = 3.0f;
			pData.transform = { {1.0f,1.0f,1.0f},0,0 };
			pData.velocity = { RandomEngine::GetRandom(-1.0f,1.0f),RandomEngine::GetRandom(-1.0f,1.0f) ,RandomEngine::GetRandom(-1.0f,1.0f) };
			particle->MakeNewParticle(pData);
		particle->Updade();
}

void GameScene::Draw2D() {
	sprite_->Draw();
}

void GameScene::Draw3D() {
	Model::PreDraw(dxCommon_->GetCommandList());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(5, srvHandleGPU);
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());

	sphere_->Draw(worldTransformSphere_, viewProjection_, monsterTextureHandle_);

	//skydome_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);
	//flooar_->Draw(viewProjection_);

	Model::PreDrawOutLine(dxCommon_->GetCommandList());
	sphere_->DrawOutLine(worldTransformSphere_, viewProjection_);
	//ground_->DrawOutLine(viewProjection_);
	Model::PostDraw();
	Particle::PreDraw(dxCommon_->GetCommandList());
	particle->Draw(viewProjection_);
	Particle::PostDraw();
}
