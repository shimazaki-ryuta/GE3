#include "WorldTransform.h"
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