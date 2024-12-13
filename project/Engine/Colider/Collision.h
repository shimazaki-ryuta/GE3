#pragma once
#include "Sphere.h"
#include "Vector3.h"
#include "Plane.h"
#include "AABB.h"
#include "OBB.h"

//形状衝突用関数群

//衝突関数
bool IsCollision(const Sphere& s, const Plane& p);
bool IsCollision(const Sphere& s, const AABB& aabb);
bool IsCollision(const Sphere& s, const OBB& obb);

bool IsCollision(const Segment& s, const Plane& p);
bool IsCollision(const Line& l, const Plane& p);
bool IsCollision(const Ray& r, const Plane& p);
bool IsCollision(const AABB& aabb1, const AABB& aabb2);
bool IsCollision(const AABB& aabb, const Sphere& sphere);
bool IsCollision(const AABB& aabb, const Segment& segment);
bool IsCollision(const AABB& aabb, const Line& segment);
bool IsCollision(const AABB& aabb, const Ray& segment);

bool IsCollision(const OBB& obb, const Sphere& sphere);
bool IsCollision(const OBB& obb, const Segment& segment);
bool IsCollision(const OBB& obb, const Line& segment);
bool IsCollision(const OBB& obb, const Ray& segment);
bool IsCollision(const OBB& obb1, const OBB& obb2);


//最近接点を求める
Vector3 GetClosestPoint(const OBB& obb, const Sphere& sphere);

//押し戻し、係数tが0だったら左だけ押し戻し
void PushBack(float t,OBB& obb,Sphere& sphere);