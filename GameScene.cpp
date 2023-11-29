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

#include "collision.h"
#include "CollisionManager.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;
}

void GameScene::Update() {
	
	Matrix4x4 rotateMatrix0 = DirectionToDIrection({ 1.0f,0.0f,0.0f }, {-1.0f,0.0f,0.0f});
	Vector3 from0 = Normalize(Vector3{1.0f,0.7f,0.5f});
	Vector3 to0 = -1.0f* from0;
	Matrix4x4 rotateMatrix1 = DirectionToDIrection(from0,to0);
	Vector3 from1 = Normalize(Vector3{ -0.6f,0.9f,0.2f });
	Vector3 to1 = Normalize(Vector3{ 0.4f,0.7f,-0.5f });
	Matrix4x4 rotateMatrix2 = DirectionToDIrection(from1, to1);
	int num = 0;
	std::function<void(const Matrix4x4& matrix)> printMatrix4x4 = [&](const Matrix4x4& matrix) {
#ifdef _DEBUG
		ImGui::Begin("RotateMatrix");
		ImGui::Text("RotateMatrix%d",num);
		ImGui::Text("%6.3f  %6.3f %6.3f %6.3f", matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3]);
		ImGui::Text("%6.3f  %6.3f %6.3f %6.3f", matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3]);
		ImGui::Text("%6.3f  %6.3f %6.3f %6.3f", matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3]);
		ImGui::Text("%6.3f  %6.3f %6.3f %6.3f", matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]);
		ImGui::End();
#endif // _DEBUG
		num++;
	};
	
	printMatrix4x4(rotateMatrix0);
	printMatrix4x4(rotateMatrix1);
	printMatrix4x4(rotateMatrix2);
}

void GameScene::Draw2D() {

}

void GameScene::Draw3D() {

}

