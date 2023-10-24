#include "OBB.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"

void SetOridentatios(OBB& obb, const Matrix4x4& rotateMatrix)
{
	obb.oridentatios[0].x = rotateMatrix.m[0][0];
	obb.oridentatios[0].y = rotateMatrix.m[0][1];
	obb.oridentatios[0].z = rotateMatrix.m[0][2];

	obb.oridentatios[1].x = rotateMatrix.m[1][0];
	obb.oridentatios[1].y = rotateMatrix.m[1][1];
	obb.oridentatios[1].z = rotateMatrix.m[1][2];

	obb.oridentatios[2].x = rotateMatrix.m[2][0];
	obb.oridentatios[2].y = rotateMatrix.m[2][1];
	obb.oridentatios[2].z = rotateMatrix.m[2][2];

}


Matrix4x4 GetRotate(const OBB& obb)
{
	Matrix4x4 rotateMatrix;
	rotateMatrix = MakeIdentity4x4();
	rotateMatrix.m[0][0] = obb.oridentatios[0].x;
	rotateMatrix.m[0][1] = obb.oridentatios[0].y;
	rotateMatrix.m[0][2] = obb.oridentatios[0].z;

	rotateMatrix.m[1][0] = obb.oridentatios[1].x;
	rotateMatrix.m[1][1] = obb.oridentatios[1].y;
	rotateMatrix.m[1][2] = obb.oridentatios[1].z;

	rotateMatrix.m[2][0] = obb.oridentatios[2].x;
	rotateMatrix.m[2][1] = obb.oridentatios[2].y;
	rotateMatrix.m[2][2] = obb.oridentatios[2].z;

	return rotateMatrix;
}
/*
void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color)
{
	Vector3 screen[8];
	//bottom
	screen[0] = Vector3{  - obb.size.x, - obb.size.y, - obb.size.z };
	screen[1] = Vector3{  + obb.size.x, - obb.size.y, - obb.size.z };
	screen[2] = Vector3{  - obb.size.x, - obb.size.y, + obb.size.z };
	screen[3] = Vector3{  + obb.size.x, - obb.size.y, + obb.size.z };
	//top
	screen[4] = Vector3{  - obb.size.x, + obb.size.y, - obb.size.z };
	screen[5] = Vector3{  + obb.size.x, + obb.size.y, - obb.size.z };
	screen[6] = Vector3{  - obb.size.x, + obb.size.y, + obb.size.z };
	screen[7] = Vector3{  + obb.size.x, + obb.size.y, + obb.size.z };

	Matrix4x4 rotateMatrix = GetRotate(obb);

	for (int index = 0; index < 8; index++)
	{
		screen[index] = screen[index] * rotateMatrix;
		screen[index] = Add(screen[index],obb.center);
		screen[index] = screen[index] * viewProjectionMatrix * viewPortMatrix;
	}

	Novice::DrawLine(int(screen[0].x), int(screen[0].y), int(screen[1].x), int(screen[1].y), color);
	Novice::DrawLine(int(screen[0].x), int(screen[0].y), int(screen[2].x), int(screen[2].y), color);
	Novice::DrawLine(int(screen[1].x), int(screen[1].y), int(screen[3].x), int(screen[3].y), color);
	Novice::DrawLine(int(screen[2].x), int(screen[2].y), int(screen[3].x), int(screen[3].y), color);

	Novice::DrawLine(int(screen[0].x), int(screen[0].y), int(screen[4].x), int(screen[4].y), color);
	Novice::DrawLine(int(screen[1].x), int(screen[1].y), int(screen[5].x), int(screen[5].y), color);
	Novice::DrawLine(int(screen[2].x), int(screen[2].y), int(screen[6].x), int(screen[6].y), color);
	Novice::DrawLine(int(screen[3].x), int(screen[3].y), int(screen[7].x), int(screen[7].y), color);

	Novice::DrawLine(int(screen[4].x), int(screen[4].y), int(screen[5].x), int(screen[5].y), color);
	Novice::DrawLine(int(screen[4].x), int(screen[4].y), int(screen[6].x), int(screen[6].y), color);
	Novice::DrawLine(int(screen[5].x), int(screen[5].y), int(screen[7].x), int(screen[7].y), color);
	Novice::DrawLine(int(screen[6].x), int(screen[6].y), int(screen[7].x), int(screen[7].y), color);
}
*/
