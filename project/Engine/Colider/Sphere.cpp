#include "Sphere.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#define _USE_MATH_DEFINES
#include <math.h>

bool IsCollision(const Sphere& s1, const Sphere& s2)
{
	float distance = Length(s2.center-s1.center);
	if (distance <= s1.radius + s2.radius)
	{
		return true;
	}
	return false;
}
