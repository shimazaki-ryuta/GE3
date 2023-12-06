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
#include "QuaternionFunction.h"
GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;
}

void GameScene::Update() {
	
	
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
	
	Quaternion rotation = MakeRotateAxisAngleQuaternion(Normalize(Vector3{1.0f,0.4f,-0.2f}),0.45f);
	Vector3 pointY = {2.1f,-0.9f,1.3f};
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation);
	Vector3 rotateByQuaternion = RotateVector(pointY,rotation);
	Vector3 rotateByMatrix = Transform(pointY,rotateMatrix);

	std::function<void(const Quaternion& q, const char label[])> printQuaternion = [&](const Quaternion& q,const char label[]) {
#ifdef _DEBUG
		ImGui::Begin("Quaternion");
		ImGui::Text("%6.2f %6.2f %6.2f %6.2f : %s", q.v.x, q.v.y, q.v.z, q.w,label);
		ImGui::End();
#endif // _DEBUG
	};

	printQuaternion(rotation,"rotation");
	printMatrix4x4(rotateMatrix);
#ifdef _DEBUG
	ImGui::Begin("Quaternion");
	ImGui::Text("%6.2f %6.2f %6.2f : %s", rotateByQuaternion.x, rotateByQuaternion.y, rotateByQuaternion.z ,"rotateByQuaternion");
	ImGui::Text("%6.2f %6.2f %6.2f : %s", rotateByMatrix.x, rotateByMatrix.y, rotateByMatrix.z, "rotateByMatrix");
	ImGui::End();
#endif // _DEBUG
}

void GameScene::Draw2D() {

}

void GameScene::Draw3D() {

}

