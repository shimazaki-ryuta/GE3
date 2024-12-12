#include "Floor.h"

//ImGui
#include "../externals/imgui/imgui.h"
#include "../externals/imgui/imgui_impl_dx12.h"
#include "../externals/imgui/imgui_impl_win32.h"

void Floor::Initialize() {
	worldTransform_.Initialize();
	modelWorldTransform_.Initialize();
	modelWorldTransform_.parent_ = &worldTransform_;
	model_.reset(Model::CreateFromOBJ("Floor"));
	model_->SetEnableLighting(2);
	model_->SetEnvironmentCoefficient(0.2f);
	size_ = {2.0f,0.0f,2.0f};
	obb_.center = worldTransform_.translation_;
	obb_.size = size_;
	SetOridentatios(obb_,MakeRotateMatrix(worldTransform_.rotation_));
	offset_ = {0.0f,0.0f,0.0f};
}

void Floor::Update(){
	
	modelWorldTransform_.scale_.x = size_.x;
	modelWorldTransform_.scale_.z = size_.z;

	obb_.center = worldTransform_.translation_;
	obb_.size = size_;
	SetOridentatios(obb_, MakeRotateMatrix(worldTransform_.rotation_));
	worldTransform_.UpdateMatrix();
	modelWorldTransform_.UpdateMatrix();
}

void Floor::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(modelWorldTransform_, viewProjection);
}