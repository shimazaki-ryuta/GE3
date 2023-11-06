#pragma once
#include "Sphere.h"
#include "Vector3.h"
#include "Plane.h"
//#include "Triangle.h"
#include "AABB.h"
#include "OBB.h"

bool IsCollision(const Sphere& s, const Plane& p);
bool IsCollision(const Sphere& s, const AABB& aabb);
bool IsCollision(const Sphere& s, const OBB& obb);

bool IsCollision(const Segment& s, const Plane& p);
bool IsCollision(const Line& l, const Plane& p);
bool IsCollision(const Ray& r, const Plane& p);
//bool IsCollision(const Triangle& t, const Segment& s);
//bool IsCollision(const Triangle& t, const Line& l);
//bool IsCollision(const Triangle& t, const Ray& r);
bool IsCollision(const AABB& aabb1, const AABB& aabb2);
bool IsCollision(const AABB& aabb, const Sphere& sphere);
bool IsCollision(const AABB& aabb, const Segment& segment);
bool IsCollision(const AABB& aabb, const Line& segment);
bool IsCollision(const AABB& aabb, const Ray& segment);
//bool IsCollision(const AABB& aabb, const OBB& obb);

bool IsCollision(const OBB& obb, const Sphere& sphere);
bool IsCollision(const OBB& obb, const Segment& segment);
bool IsCollision(const OBB& obb, const Line& segment);
bool IsCollision(const OBB& obb, const Ray& segment);
bool IsCollision(const OBB& obb1, const OBB& obb2);