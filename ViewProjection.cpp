#include "ViewProjection.h"
#include "MatrixFunction.h"

void ViewProjection::Initialize(int kClientWidth,int kClientHeight)
{
	matView = Inverse(MakeIdentity4x4());
	matProjection = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
}