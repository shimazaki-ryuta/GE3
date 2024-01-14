#include "Curve.h"
//#include "PrimitiveDrawer.h"
#include <algorithm>

Vector3 CatmullRom(
    const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
	Vector3 vector;
	vector.x = (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * powf(t, 3) +
	           (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * powf(t, 2) + (-p0.x + p2.x) * t + 2 * p1.x;
	vector.y = (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * powf(t, 3) +
	           (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * powf(t, 2) + (-p0.y + p2.y) * t + 2 * p1.y;
	vector.z = (-p0.z + 3 * p1.z - 3 * p2.z + p3.z) * powf(t, 3) +
	           (2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z) * powf(t, 2) + (-p0.z + p2.z) * t + 2 * p1.z;
	return vector * (1.0f / 2.0f);
}

Vector3 CatmullRomComplement(std::vector<Vector3> controlPoints,float t) 
{ 
	int pointsCount = int(controlPoints.size())-1;
	float divPoint = 1.0f / float(pointsCount);
	int section = int(t/divPoint);
	//float segmentT = t-(divPoint*float(section));
	float segmentT = (t - float(section) * divPoint) /
	                 (float(section + 1) * divPoint - float(section) * divPoint);
	return CatmullRom(
	    controlPoints[std::clamp(section - 1, 0, pointsCount)], controlPoints[section],
	    controlPoints[std::clamp(section + 1, 0, pointsCount)],
	    controlPoints[std::clamp(section + 2, 0, pointsCount)], segmentT);
}

void DrawCurve(std::vector<Vector3> controlPoints, ViewProjection& viewProjection)
{
	std::vector<Vector3> pointsDrawing;
	const size_t segmentCount = 200;
	for (size_t i=0; i<segmentCount+1;i++)
	{
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomComplement(controlPoints,t);
		pointsDrawing.push_back(pos);
	}
	//PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection);
	for (size_t i = 0; i < segmentCount; i++) {
		//PrimitiveDrawer::GetInstance()->DrawLine3d(pointsDrawing[i], pointsDrawing[i + 1], Vector4{1.0f,0.0f,0.0f,1.0f});
	}

}
