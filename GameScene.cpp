#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <list>
#include <fstream>
#include  "Primitive3D.h"
//ImGui
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;
	input_ = Input::GetInstance();


	model_.Create("resources", "fence.obj");

	worldTransformObj_.Initialize();
	worldTransformObj2_.Initialize();
	viewProjection_.Initialize(1280,720);
	viewProjection_.translation_.z = -10;
}

void GameScene::Update() {
	ImGui::Begin("obj1");
	ImGui::DragFloat3("Scale", &worldTransformObj_.scale_.x, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat3("Rotate", &worldTransformObj_.rotation_.x, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat3("Translate", &worldTransformObj_.translation_.x, 1.0f,-1000.0f, 1000.0f);
	ImGui::End();
	ImGui::Begin("obj2");
	ImGui::DragFloat3("Scale", &worldTransformObj2_.scale_.x, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat3("Rotate", &worldTransformObj2_.rotation_.x, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat3("Translate", &worldTransformObj2_.translation_.x, 1.0f, - 1000.0f, 1000.0f); ImGui::End();


	worldTransformObj_.UpdateMatrix();
	worldTransformObj2_.UpdateMatrix();

	viewProjection_.UpdateMatrix();
}

void GameScene::Draw2D() {

}

void GameScene::Draw3D() {
	Primitive3D::PreDraw(dxCommon_->GetCommandList());
	Primitive3D::PostDraw();
	Model::PreDraw(dxCommon_->GetCommandList());
	model_.Draw(worldTransformObj_, viewProjection_);
//	model_.Draw(worldTransformObj2_, viewProjection_);

	Model::PostDraw();
}

