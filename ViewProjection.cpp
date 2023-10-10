#include "ViewProjection.h"
#include "MatrixFunction.h"

void ViewProjection::Initialize(int kClientWidth,int kClientHeight)
{
	matView = Inverse(MakeIdentity4x4());
	matProjection = MakePerspectiveFovMatrix(fovAngleY, float(kClientWidth) / float(kClientHeight), nearZ, farZ);
	width = float(kClientWidth);
	height = float(kClientHeight);

}

void ViewProjection::UpdateMatrix()
{
	matView = Inverse(MakeAffineMatrix({1.0f,1.0f,1.0f},rotation_,translation_));
	matProjection = MakePerspectiveFovMatrix(fovAngleY, width / height, nearZ, farZ);
}