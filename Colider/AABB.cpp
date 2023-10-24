#include "AABB.h"
//#include <Novice.h>
#include "MatrixFunction.h"
#include <cmath>

AABB Normalize(const AABB& aabb)
{
	AABB aabb2;
	aabb2.min.x = (std::min)(aabb.min.x,aabb.max.x);
	aabb2.min.y = (std::min)(aabb.min.y, aabb.max.y);
	aabb2.min.z = (std::min)(aabb.min.z, aabb.max.z);
	aabb2.max.x = (std::max)(aabb.min.x, aabb.max.x);
	aabb2.max.y = (std::max)(aabb.min.y, aabb.max.y);
	aabb2.max.z = (std::max)(aabb.min.z, aabb.max.z);
	return aabb2;
}
/*
void DrawAABB(const AABB& aabb,const Matrix4x4& viewProjectionMatrix,const Matrix4x4& viewPortMatrix,uint32_t color)
{
	Vector3 screen[8];
	//bottom
	screen[0] = Vector3{ aabb.min.x,aabb.min.y,aabb.min.z };
	screen[1] = Vector3{ aabb.max.x,aabb.min.y,aabb.min.z };
	screen[2] = Vector3{ aabb.min.x,aabb.min.y,aabb.max.z };
	screen[3] = Vector3{ aabb.max.x,aabb.min.y,aabb.max.z };
	//top
	screen[4] = Vector3{ aabb.min.x,aabb.max.y,aabb.min.z };
	screen[5] = Vector3{ aabb.max.x,aabb.max.y,aabb.min.z };
	screen[6] = Vector3{ aabb.min.x,aabb.max.y,aabb.max.z };
	screen[7] = Vector3{ aabb.max.x,aabb.max.y,aabb.max.z };

	for (int index = 0;index < 8;index++)
	{
		screen[index] = screen[index] * viewProjectionMatrix * viewPortMatrix;
	}
	
	Novice::DrawLine(int(screen[0].x), int(screen[0].y), int(screen[1].x), int(screen[1].y),color);
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