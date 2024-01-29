#include "Collision.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include <cmath>
#include <algorithm>
bool IsCollision(const Sphere& s, const Plane& p)
{
	float k = (Dot(p.nomal,s.center)-p.distance);
	k = std::sqrtf(std::powf(k,2));
	Vector3 q = s.center - k * p.nomal;
	float distance = Length(q-s.center);
	if (distance <= s.radius)
	{
		return true;
	}
	return false;
}

bool IsCollision(const Segment& s, const Plane& p)
{
	float dot = Dot(s.diff, p.nomal);
	if (dot == 0)
	{
		return false;
	}
	float t = (p.distance - Dot(s.origin,p.nomal))/dot;
	if (t >= 0.0f && t <= 1.0f)
	{
		return true;
	}
	return false;
}

bool IsCollision(const Line& l, const Plane& p)
{
	float dot = Dot(l.diff, p.nomal);
	if (dot == 0)
	{
		return false;
	}
	return true;
}

bool IsCollision(const Ray& r, const Plane& p)
{
	float dot = Dot(r.diff, p.nomal);
	if (dot == 0)
	{
		return false;
	}
	float t = (p.distance - Dot(r.origin, p.nomal)) / dot;
	if (t >= 0.0f)
	{
		return true;
	}
	return false;
}
/*
bool IsCollision(const Triangle& triangle, const Segment& s)
{
	Plane plane;
	Vector3 v1=(triangle.vertices[1] - triangle.vertices[0]), v2=(triangle.vertices[2]-triangle.vertices[1]),v3=(triangle.vertices[0]-triangle.vertices[2]);
	plane.nomal = Normalize(Cross(v1,v2));
	plane .distance = Dot(triangle.vertices[0], plane.nomal);

	if (!IsCollision(s, plane))
	{
		return false;
	}
	float dot = Dot(s.diff, plane.nomal);
	float t = (plane.distance - Dot(s.origin, plane.nomal)) / dot;
	
	Vector3 p = s.origin + t * s.diff;
	Vector3 cross1 = Cross(v1, Vector3(p - triangle.vertices[1]));
	Vector3 cross2 = Cross(v2, Vector3(p - triangle.vertices[2]));
	Vector3 cross3 = Cross(v3, Vector3(p - triangle.vertices[0]));
	float dot1 = Dot(cross1, plane.nomal);
	float dot2 = Dot(cross2, plane.nomal);
	float dot3 = Dot(cross3, plane.nomal);
	if (dot1 >= 0.0f && dot2 >= 0.0f && dot3 >= 0.0f)
	{
		return true;
	}
	return false;
}

bool IsCollision(const Triangle& triangle, const Line& l)
{
	Plane plane;
	Vector3 v1 = (triangle.vertices[1] - triangle.vertices[0]), v2 = (triangle.vertices[2] - triangle.vertices[1]), v3 = (triangle.vertices[0] - triangle.vertices[2]);
	plane.nomal = Normalize(Cross(v1, v2));
	plane.distance = Dot(triangle.vertices[0], plane.nomal);

	if (!IsCollision(l, plane))
	{
		return false;
	}
	float dot = Dot(l.diff, plane.nomal);
	float t = (plane.distance - Dot(l.origin, plane.nomal)) / dot;

	Vector3 p = l.origin + t * l.diff;
	Vector3 cross1 = Cross(v1, Vector3(p - triangle.vertices[1]));
	Vector3 cross2 = Cross(v2, Vector3(p - triangle.vertices[2]));
	Vector3 cross3 = Cross(v3, Vector3(p - triangle.vertices[0]));
	float dot1 = Dot(cross1, plane.nomal);
	float dot2 = Dot(cross2, plane.nomal);
	float dot3 = Dot(cross3, plane.nomal);
	if (dot1 >= 0.0f && dot2 >= 0.0f && dot3 >= 0.0f)
	{
		return true;
	}
	return false;
}

bool IsCollision(const Triangle& triangle, const Ray& r)
{
	Plane plane;
	Vector3 v1 = (triangle.vertices[1] - triangle.vertices[0]), v2 = (triangle.vertices[2] - triangle.vertices[1]), v3 = (triangle.vertices[0] - triangle.vertices[2]);
	plane.nomal = Normalize(Cross(v1, v2));
	plane.distance = Dot(triangle.vertices[0], plane.nomal);

	if (!IsCollision(r, plane))
	{
		return false;
	}
	float dot = Dot(r.diff, plane.nomal);
	float t = (plane.distance - Dot(r.origin, plane.nomal)) / dot;

	Vector3 p = r.origin + t * r.diff;
	Vector3 cross1 = Cross(v1, Vector3(p - triangle.vertices[1]));
	Vector3 cross2 = Cross(v2, Vector3(p - triangle.vertices[2]));
	Vector3 cross3 = Cross(v3, Vector3(p - triangle.vertices[0]));
	float dot1 = Dot(cross1, plane.nomal);
	float dot2 = Dot(cross2, plane.nomal);
	float dot3 = Dot(cross3, plane.nomal);
	if (dot1 >= 0.0f && dot2 >= 0.0f && dot3 >= 0.0f)
	{
		return true;
	}
	return false;
}
*/
bool IsCollision(const AABB& aabb1, const AABB& aabb2)
{
	AABB normalized1 = Normalize(aabb1) ,normalized2 = Normalize(aabb2);
	if ((normalized1.min.x <= normalized2.max.x && normalized1.max.x >= normalized2.min.x) && 
		(normalized1.min.y <= normalized2.max.y && normalized1.max.y >= normalized2.min.y) && 
		(normalized1.min.z <= normalized2.max.z && normalized1.max.z >= normalized2.min.z))
	{
		return true;
	}
	return false;
}

bool IsCollision(const AABB& aabb, const Sphere& sphere)
{
	Vector3 closestPoint{std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y) ,
		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z) };

	float distance = Length(closestPoint - sphere.center);
	if (distance <= sphere.radius)
	{
		return true;
	}
	return false;
}

bool IsCollision(const AABB& aabb, const Segment& segment)
{
	float tXmin = (aabb.min.x - segment.origin.x) / segment.diff.x;
	float tXmax = (aabb.max.x - segment.origin.x) / segment.diff.x;
	float tYmin = (aabb.min.y - segment.origin.y) / segment.diff.y;
	float tYmax = (aabb.max.y - segment.origin.y) / segment.diff.y;
	float tZmin = (aabb.min.z - segment.origin.z) / segment.diff.z;
	float tZmax = (aabb.max.z - segment.origin.z) / segment.diff.z;

	float tNearX = std::min(tXmin, tXmax);
	float tNearY = std::min(tYmin, tYmax);
	float tNearZ = std::min(tZmin, tZmax);

	float tFarX = std::max(tXmin, tXmax);
	float tFarY = std::max(tYmin, tYmax);
	float tFarZ = std::max(tZmin, tZmax);

	float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
	float tmax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (tmin<= tmax)
	{
		if (tmin<=1.0f && tmax>=0.0f)
		{
			return true;
		}
	}
	return false;
}

bool IsCollision(const AABB& aabb, const Line& segment)
{
	float tXmin = (aabb.min.x - segment.origin.x) / segment.diff.x;
	float tXmax = (aabb.max.x - segment.origin.x) / segment.diff.x;
	float tYmin = (aabb.min.y - segment.origin.y) / segment.diff.y;
	float tYmax = (aabb.max.y - segment.origin.y) / segment.diff.y;
	float tZmin = (aabb.min.z - segment.origin.z) / segment.diff.z;
	float tZmax = (aabb.max.z - segment.origin.z) / segment.diff.z;

	float tNearX = std::min(tXmin, tXmax);
	float tNearY = std::min(tYmin, tYmax);
	float tNearZ = std::min(tZmin, tZmax);

	float tFarX = std::max(tXmin, tXmax);
	float tFarY = std::max(tYmin, tYmax);
	float tFarZ = std::max(tZmin, tZmax);

	float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
	float tmax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (tmin <= tmax)
	{
		return true;
	}
	return false;
}

bool IsCollision(const AABB& aabb, const Ray& segment)
{
	float tXmin = (aabb.min.x - segment.origin.x) / segment.diff.x;
	float tXmax = (aabb.max.x - segment.origin.x) / segment.diff.x;
	float tYmin = (aabb.min.y - segment.origin.y) / segment.diff.y;
	float tYmax = (aabb.max.y - segment.origin.y) / segment.diff.y;
	float tZmin = (aabb.min.z - segment.origin.z) / segment.diff.z;
	float tZmax = (aabb.max.z - segment.origin.z) / segment.diff.z;

	float tNearX = std::min(tXmin, tXmax);
	float tNearY = std::min(tYmin, tYmax);
	float tNearZ = std::min(tZmin, tZmax);

	float tFarX = std::max(tXmin, tXmax);
	float tFarY = std::max(tYmin, tYmax);
	float tFarZ = std::max(tZmin, tZmax);

	float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
	float tmax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (tmin <= tmax)
	{
		if (tmin <= 1.0f )
		{
			return true;
		}
	}
	return false;
}

bool IsCollision(const OBB& obb, const Sphere& sphere)
{
	AABB aabb;
	aabb.min.x = -obb.size.x;
	aabb.min.y = -obb.size.y;
	aabb.min.z = -obb.size.z;
	aabb.max.x = obb.size.x;
	aabb.max.y = obb.size.y;
	aabb.max.z = obb.size.z;

	Matrix4x4 worldInverse = Inverse(GetRotate(obb)*MakeTranslateMatrix(obb.center));
	Sphere localSphere;
	localSphere.center = sphere.center * worldInverse;
	localSphere.radius = sphere.radius;
	return IsCollision(aabb, localSphere);
}

bool IsCollision(const OBB& obb, const Segment& segment)
{
	AABB localAABB{ -1.0f*obb.size,obb.size };
	Matrix4x4 worldInverse = Inverse(GetRotate(obb) * MakeTranslateMatrix(obb.center));
	auto localLine = segment;
	localLine.origin = segment.origin * worldInverse;
	Vector3 localLineEnd = (segment.origin + segment.diff) * worldInverse;
	localLine.diff = localLineEnd - localLine.origin;
	return IsCollision(localAABB,localLine);
}

bool IsCollision(const OBB& obb, const Line& segment)
{
	AABB localAABB{ -1.0f * obb.size,obb.size };
	Matrix4x4 worldInverse = Inverse(GetRotate(obb) * MakeTranslateMatrix(obb.center));
	auto localLine = segment;
	localLine.origin = segment.origin * worldInverse;
	Vector3 localLineEnd = (segment.origin + segment.diff) * worldInverse;
	localLine.diff = localLineEnd - localLine.origin;
	return IsCollision(localAABB, localLine);
}

bool IsCollision(const OBB& obb, const Ray& segment)
{
	AABB localAABB{ -1.0f * obb.size,obb.size };
	Matrix4x4 worldInverse = Inverse(GetRotate(obb) * MakeTranslateMatrix(obb.center));
	auto localLine = segment;
	localLine.origin = segment.origin * worldInverse;
	Vector3 localLineEnd = (segment.origin + segment.diff) * worldInverse;
	localLine.diff = localLineEnd - localLine.origin;
	return IsCollision(localAABB, localLine);
}

bool IsCollision(const OBB& obb1, const OBB& obb2)
{
	Vector3 separatingAxis[15];
	separatingAxis[0] = obb1.oridentatios[0];
	separatingAxis[1] = obb1.oridentatios[1];
	separatingAxis[2] = obb1.oridentatios[2];
	separatingAxis[3] = obb2.oridentatios[0];
	separatingAxis[4] = obb2.oridentatios[1];
	separatingAxis[5] = obb2.oridentatios[2];
	int axisNum = 6;
	for (int index1 = 0; index1 < 3; index1++)
	{
		for (int index2 = 0; index2 < 3; index2++)
		{
			separatingAxis[axisNum] = Cross(obb1.oridentatios[index1], obb2.oridentatios[index2]);
			axisNum++;
		}
	}

	Vector3 obb1Vetyex[8];
	//bottom
	obb1Vetyex[0] = Vector3{ -obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vetyex[1] = Vector3{ +obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vetyex[2] = Vector3{ -obb1.size.x, -obb1.size.y, +obb1.size.z };
	obb1Vetyex[3] = Vector3{ +obb1.size.x, -obb1.size.y, +obb1.size.z };
	//top
	obb1Vetyex[4] = Vector3{ -obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vetyex[5] = Vector3{ +obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vetyex[6] = Vector3{ -obb1.size.x, +obb1.size.y, +obb1.size.z };
	obb1Vetyex[7] = Vector3{ +obb1.size.x, +obb1.size.y, +obb1.size.z };

	Matrix4x4 rotateMatrix1 = GetRotate(obb1);

	Vector3 obb2Vetyex[8];
	//bottom
	obb2Vetyex[0] = Vector3{ -obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vetyex[1] = Vector3{ +obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vetyex[2] = Vector3{ -obb2.size.x, -obb2.size.y, +obb2.size.z };
	obb2Vetyex[3] = Vector3{ +obb2.size.x, -obb2.size.y, +obb2.size.z };
	//top
	obb2Vetyex[4] = Vector3{ -obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vetyex[5] = Vector3{ +obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vetyex[6] = Vector3{ -obb2.size.x, +obb2.size.y, +obb2.size.z };
	obb2Vetyex[7] = Vector3{ +obb2.size.x, +obb2.size.y, +obb2.size.z };

	Matrix4x4 rotateMatrix2 = GetRotate(obb2);

	for (int index = 0; index < 8; index++)
	{
		obb1Vetyex[index] = obb1Vetyex[index] * rotateMatrix1;
		obb1Vetyex[index] = Add(obb1Vetyex[index], obb1.center);
		obb2Vetyex[index] = obb2Vetyex[index] * rotateMatrix2;
		obb2Vetyex[index] = Add(obb2Vetyex[index], obb2.center);
	}

	for (axisNum = 0; axisNum < 15; axisNum++)
	{
		float projectionPoint1[8];
		float projectionPoint2[8];
		float min1, max1;
		float min2, max2;
		for (int index = 0; index < 8; index++)
		{
			projectionPoint1[index] = Dot(obb1Vetyex[index], Normalize(separatingAxis[axisNum]));
			projectionPoint2[index] = Dot(obb2Vetyex[index], Normalize(separatingAxis[axisNum]));
			if (index == 0)
			{
				min1 = projectionPoint1[index];
				min2 = projectionPoint2[index];
				max1 = min1;
				max2 = min2;
			}
			else
			{
				min1 = std::min(min1, projectionPoint1[index]);
				min2 = std::min(min2, projectionPoint2[index]);
				max1 = std::max(max1, projectionPoint1[index]);
				max2 = std::max(max2, projectionPoint2[index]);
			}
		}
		float L1 = max1 - min1;
		float L2 = max2 - min2;
		float sumSpan = L1 + L2;
		float longSpan = std::max(max1, max2) - std::min(min1, min2);
		if (sumSpan < longSpan)
		{
			return false;
		}
	}
	return true;
}

bool IsCollision(const Sphere& s, const AABB& aabb) {
	return IsCollision(aabb,s);
}
bool IsCollision(const Sphere& s, const OBB& obb) {
	return IsCollision(obb, s);
}
/*
bool IsCollision(const AABB& aabb, const OBB& obb) {
	return IsCollision(obb, aabb);
}
*/