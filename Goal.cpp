#include "Goal.h"
#include "TextureManager.h"
//ImGui
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

void Goal::Initialize() {
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f,1.5f,110.0f};
	model_.reset(Model::CreateFromOBJ("Cube"));
	textureHandle_ = TextureManager::LoadTexture("g.png");
	size_ = { 1.0f,1.0f,1.0f };
	obb_.center = worldTransform_.translation_;
	obb_.size = size_;
	SetOridentatios(obb_, MakeRotateMatrix(worldTransform_.rotation_));
	offset_ = { 0.0f,0.0f,0.0f };
}

void Goal::Update() {
	/*ImGui::Begin("goal");
	ImGui::DragFloat3("position",&worldTransform_.translation_.x,0.1f);
	ImGui::End();
	*/
	
	obb_.center = worldTransform_.translation_;
	obb_.size = size_;
	SetOridentatios(obb_, MakeRotateMatrix(worldTransform_.rotation_));
	worldTransform_.UpdateMatrix();
	
}

void Goal::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection,textureHandle_);
}