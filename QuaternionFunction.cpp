#include "QuaternionFunction.h"
#include "VectorFunction.h"
#include <cmath>
Quaternion Multiply(const Quaternion& q, const Quaternion& r) {
	Quaternion result;
	result.w = q.w * r.w - Dot(q.v,r.v);
	result.v = Cross(q.v, r.v) + r.w * q.v+ q.w*r.v;
	return result;
}

Quaternion IdentityQuaternion() {
	return Quaternion{1.0f,0,0,0};
}

Quaternion Conjugate(const Quaternion& q) {
	return Quaternion{( - 1.0f * q.v),q.w };
}

float Norm(const Quaternion& q) {
	return std::sqrtf(q.w*q.w + q.v.x * q.v.x + q.v.y * q.v.y + q.v.z * q.v.z);
}

Quaternion Normalize(const Quaternion& q) {
	Quaternion result = q;
	float norm = Norm(q);
	result.w /= norm;
	result.v.x /= norm;
	result.v.y /= norm;
	result.v.z /= norm;
	return result;
}

Quaternion Inverse(const Quaternion& q) {
	Quaternion result = Conjugate(q);
	float length = Norm(q) * Norm(q);
	result.w /= length;
	result.v.x /= length;
	result.v.y /= length;
	result.v.z /= length;
	return result;
}
