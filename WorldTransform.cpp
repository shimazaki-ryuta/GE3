#include "WorldTransform.h"
#include"MatrixFunction.h"
void WorldTransform::Initialize()
{
	UpdateMatrix();
}
void WorldTransform::UpdateMatrix()
{
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	if (parent_)
	{
		matWorld_ *= parent_->matWorld_;
	}
}
