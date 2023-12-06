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
	
	Quaternion q1 = { 2.0f,3.0f,4.0f,1.0f };
	Quaternion q2 = { 1.0f,3.0f,5.0f,2.0f };
	Quaternion identity = IdentityQuaternion();
	Quaternion conj = Conjugate(q1);
	Quaternion inv = Inverse(q1);
	Quaternion normal = Normalize(q1);
	Quaternion mul1 = Multiply(q1, q2);
	Quaternion mul2 = Multiply(q2, q1);
	float norm = Norm(q1);

	std::function<void(const Quaternion& q, const char label[])> printQuaternion = [&](const Quaternion& q,const char label[]) {
#ifdef _DEBUG
		ImGui::Begin("Quaternion");
		ImGui::Text("%6.2f %6.2f %6.2f %6.2f : %s", q.v.x, q.v.y, q.v.z, q.w,label);
		ImGui::End();
#endif // _DEBUG
	};

	printQuaternion(identity,"Identity");
	printQuaternion(conj,"Conjugate");
	printQuaternion(inv,"Inverse");
	printQuaternion(normal,"Normalize");
	printQuaternion(mul1,"Multiply(q1,q2)");
	printQuaternion(mul2, "Multiply(q2,q1)");
#ifdef _DEBUG
	ImGui::Begin("Quaternion");
	ImGui::Text("%6.2f : %s",norm,"Norm");
	ImGui::End();
#endif // _DEBUG
}

void GameScene::Draw2D() {

}

void GameScene::Draw3D() {

}

