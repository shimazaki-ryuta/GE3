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
	worldTransform_.scale_ = {3.0f,3.0f,3.0f};
	worldTransform_.translation_.y = 0.0f;
	worldTransform_.UpdateMatrix();
	model_->SetEnableLighting(2);
	model_->SetShiniess(40.0f);
	model_->SetShadingType(0);
	float width = 0.0125f;
	model_->SetOutLineWidth({ width,width ,width });
	model_->SetOutLineColor({0.0f,1.0f,0.0f,1.0f});
	model_->SetToonShadowTextureHandle(TextureManager::LoadTexture("toonShadow1.png"));
	material_.reset(new Material);
	material_->Initialize();
	material_->paramater_.enableLighting = 2;
	material_->paramater_.disolveColor = Vector4{ 5.2f, 0.2f, 0.2f, 0.0f };
	//model_->SetGrowStrength(0.5f);
	testAnimation_.reset(new Animation);
	testAnimation_->Initialize();
	testAnimation_->LoadAnimationFile("Resources/human", "sneakWalk.gltf");
	testAnimation_->SetPlaySpeed(100.0f);
	testSkeleton_.reset(new Skeleton);
	testSkeleton_->Initialize(model->GetRootNode());
	cluster_ = LoadModel::CreateSkinCluster(testSkeleton_->GetSkeletonData(),model->GetModelData());
}

void Ground::Update() {
	static float st;
	ImGui::Begin("animationmodel");
	ImGui::DragFloat("Speed",&t,0.1f);
	ImGui::DragFloat3("Scale",&worldTransform_.scale_.x,0.1f,0.0f,5.0f);
	ImGui::DragFloat("Shiness", &(material_->paramater_.shininess), 0.1f,0.0f,100.0f);
	ImGui::DragFloat("EnvironmentSpecuraScale",&st,0.01f,0.0f,2.0f);
	ImGui::DragFloat("Disolve",&(material_->paramater_.disolveThreshold),0.001f,0.0f,1.0f);
	ImGui::ColorEdit3("DisolveColor",&(material_->paramater_.disolveColor.x));
	ImGui::End();
	testAnimation_->SetPlaySpeed(t);
	worldTransform_.UpdateMatrix();
	model_->SetGrowStrength(growStrength_);
	//model_->SetEnvironmentCoefficient(st);
	material_->paramater_.environmentCoefficient = st;
	material_->ApplyParamater();
	model_->SetMaterial(material_.get());
}

void Ground::Draw(const ViewProjection& viewProjection) {
	testAnimation_->Update();
	testSkeleton_->ApplyAnimation(*testAnimation_->GetAnimationData().get(), testAnimation_->GetTime());
	testSkeleton_->Update();
	LoadModel::UpdateSkinCluster(cluster_,testSkeleton_->GetSkeletonData());
	model_->Draw(worldTransform_, viewProjection,cluster_);
	//testSkeleton_->Draw(worldTransform_,viewProjection);
}
void Ground::DrawOutLine(const ViewProjection& viewProjection) {
	model_->DrawOutLine(worldTransform_, viewProjection, cluster_);
}