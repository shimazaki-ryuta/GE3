#include "DebugCamera.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include "Input.h"
#include <algorithm>
#include <numbers>

//ImGui
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

void DebugCamera::Initialize()
{
	viewProjection_.Initialize();
	viewProjection_.translation_.z = -10.0f;
	isUses_ = false;
}
void DebugCamera::Update()
{
#ifdef _DEBUG
	if (isUses_) {
		ImGui::Begin("DebugCamera");
		ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
		ImGui::DragFloat3("translate", &viewProjection_.translation_.x, 0.1f);
		ImGui::End();
	}
	
#endif // _DEBUG
	


	if (target_)
	{
		Vector3 offset = { 0.0f,2.0f,-20.0f };

		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_.rotation_);

		offset = TransformNormal(offset, rotateMatrix);

		viewProjection_.translation_ = target_->GetWorldPosition() + offset;
	}

	viewProjection_.UpdateMatrix();
}
