#include "Plane.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
//#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

Vector3 Perpendicular(const Vector3& vector)
{
	if (vector.x !=0.0f || vector.y != 0.0f)
	{
		return {-vector.y,vector.x,0.0f};
	}
	return {0.0f,-vector.z,vector.y};
}
/*
void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
	Vector3 center = Multiply(plane.distance,plane.nomal);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.nomal));
	perpendiculars[1] = -1.0f * perpendiculars[0];
	perpendiculars[2] = Cross(plane.nomal,perpendiculars[0]);
	perpendiculars[3] = -1.0f * perpendiculars[2];

	Vector3 points[4];
	for (int32_t index = 0;index <4;++index)
	{
		Vector3 extend = 2.0f * perpendiculars[index];
		Vector3 point = center + extend;
		points[index] = Transform(Transform(point,viewProjectionMatrix),viewportMatrix);
	}
	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y),color);
	Novice::DrawLine(int(points[2].x), int(points[2].y), int(points[1].x), int(points[1].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), color);
}
*/