#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <list>
#include <fstream>
#include  "Primitive3D.h"

GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;
	input_ = Input::GetInstance();

}

void GameScene::Update() {


}

void GameScene::Draw2D() {

}

void GameScene::Draw3D() {
	Primitive3D::PreDraw(dxCommon_->GetCommandList());
	Primitive3D::PostDraw();
}

