#include "Ground.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
void Ground::Initialize(Model* model, const Vector3& position) {
	model_.reset(model);
	worldTransform_.Initialize();
	//worldTransform_.translation_ = position;
	worldTransform_.scale_ = {2.0f,2.0f,2.0f};
	worldTransform_.UpdateMatrix();
	model_->SetEnableLighting(2);
	model_->SetShiniess(40.0f);
	model_->SetShadingType(1);
	model_->SetToonShadowTextureHandle(TextureManager::LoadTexture("toonShadow1.png"));
	testAnimation_.reset(new Animation);
	testAnimation_->Initialize();
	testAnimation_->LoadAnimationFile("Resources/human", "walk.gltf");
	testAnimation_->SetPlaySpeed(0.0f);
	testSkeleton_.reset(new Skeleton);
	testSkeleton_->Initialize(model->GetRootNode());
	cluster_ = LoadModel::CreateSkinCluster(testSkeleton_->GetSkeletonData(),model->GetModelData());
}

void Ground::Update() {
}

void Ground::Draw(const ViewProjection& viewProjection) {
	testAnimation_->Update();
	testSkeleton_->ApplyAnimation(*testAnimation_->GetAnimationData().get(), testAnimation_->GetTime());
	testSkeleton_->Update();
	LoadModel::UpdateSkinCluster(cluster_,testSkeleton_->GetSkeletonData());
	//model_->SetLocalMatrix(testAnimation_->GetAnimationMatrix("center"));
	model_->Draw(worldTransform_, viewProjection,cluster_);
	//testSkeleton_->Draw(worldTransform_,viewProjection);
}
void Ground::DrawOutLine(const ViewProjection& viewProjection) {
	model_->DrawOutLine(worldTransform_, viewProjection);
}