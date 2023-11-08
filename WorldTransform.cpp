#include "WorldTransform.h"
#include "VectorFunction.h"
#include"MatrixFunction.h"
#include "CommonFiles/DirectXCommon.h"
ID3D12Device* WorldTransform::sDevice = nullptr;
void WorldTransform::SetDevice(ID3D12Device* device)
{
	sDevice = device;
}
void WorldTransform::Initialize()
{
	UpdateMatrix();
	transformResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(TransformationMatrix));

	transformResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
}
void WorldTransform::UpdateMatrix()
{
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	if (parent_)
	{
		matWorld_ *= parent_->matWorld_;
	}
}

void WorldTransform::TransfarMatrix(const Matrix4x4& matrix)
{
	transformationMatrixData->WVP = matWorld_ * matrix;
	transformationMatrixData->World = matWorld_;
}

Vector3 WorldTransform::GetWorldPosition() {
	return Vector3{matWorld_.m[3][0],matWorld_.m[3][1] ,matWorld_.m[3][2] };
}

Matrix4x4 WorldTransform::GetRotate() {
	Matrix4x4 rotate = matWorld_;
	rotate.m[3][0] = 0;
	rotate.m[3][1] = 0;
	rotate.m[3][2] = 0;
	Vector3 axis;
	for (int32_t index = 0; index < 3; index++) {
		axis = { rotate.m[index][0],rotate.m[index][1],rotate.m[index][2] };
		float length = Length(axis);
		rotate.m[index][0] /= length;
		rotate.m[index][1] /= length;
		rotate.m[index][2] /= length;
	}
	return rotate;
}