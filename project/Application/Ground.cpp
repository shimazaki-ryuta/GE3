#include "Ground.h"
#include "DirectXCommon.h"
#include "TextureManager.h"

//ImGui
#include "../externals/imgui/imgui.h"
#include "../externals/imgui/imgui_impl_dx12.h"
#include "../externals/imgui/imgui_impl_win32.h"

void Ground::Initialize(Model* model, const Vector3& position) {
	model_.reset(model);
	worldTransform_.Initialize();
	//worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f,1.0f,1.0f};
	worldTransform_.translation_.y = 0.0f;
	worldTransform_.UpdateMatrix();
	model_->SetEnableLighting(2);
	model_->SetShiniess(40.0f);
	model_->SetShadingType(1);
	float width = 0.005f;
	model_->SetOutLineWidth({ width,width ,width });
	model_->SetOutLineColor({0.0f,1.0f,0.0f,1.0f});
	model_->SetToonShadowTextureHandle(TextureManager::LoadTexture("toonShadow1.png"));
	//model_->SetGrowStrength(1.0f);
	testAnimation_.reset(new Animation);
	testAnimation_->Initialize();
	testAnimation_->LoadAnimationFile("Resources/human", "walk.gltf");
	testAnimation_->SetPlaySpeed(100.0f);
	testSkeleton_.reset(new Skeleton);
	testSkeleton_->Initialize(model->GetRootNode());
	cluster_ = LoadModel::CreateSkinCluster(testSkeleton_->GetSkeletonData(),model->GetModelData());
}

void Ground::Update() {
	ImGui::Begin("animationmodel");
	ImGui::DragFloat("Speed",&t,0.1f);
	ImGui::End();
	testAnimation_->SetPlaySpeed(t);
}

void Ground::Draw(const ViewProjection& viewProjection) {
	testAnimation_->Update();
	testSkeleton_->ApplyAnimation(*testAnimation_->GetAnimationData().get(), testAnimation_->GetTime());
	testSkeleton_->Update();
	LoadModel::UpdateSkinCluster(cluster_,testSkeleton_->GetSkeletonData());
	//model_->SetLocalMatrix(testAnimation_->GetAnimationMatrix("center"));
	//model_->Draw(worldTransform_, viewProjection,cluster_);
	testSkeleton_->Draw(worldTransform_,viewProjection);
}
void Ground::DrawOutLine(const ViewProjection& viewProjection) {
	model_->DrawOutLine(worldTransform_, viewProjection, cluster_);
}