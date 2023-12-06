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
	return Quaternion{0,0,0,1.0f};
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

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& vector, float angle) {
	Quaternion result;
	result.w = std::cos(angle/2.0f);
	result.v = std::sin(angle / 2.0f) * Normalize(vector);
	return result;
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& q) {
	Quaternion result = {vector,0};
	result = Multiply(q, Multiply(result,Conjugate(q)));
	return result.v;
}

Matrix4x4 MakeRotateMatrix(const Quaternion& q) {
	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = q.w * q.w + q.v.x * q.v.x - q.v.y * q.v.y - q.v.z * q.v.z;
	result.m[0][1] = 2.0f * (q.v.x * q.v.y + q.w * q.v.z);
	result.m[0][2] = 2.0f * (q.v.x * q.v.z - q.w * q.v.y);
	
	result.m[1][0] = 2.0f * (q.v.x * q.v.y - q.w * q.v.z);
	result.m[1][1] = q.w * q.w - q.v.x * q.v.x + q.v.y * q.v.y - q.v.z * q.v.z;
	result.m[1][2] = 2.0f * (q.v.y * q.v.z + q.w * q.v.x);

	result.m[2][0] = 2.0f * (q.v.x * q.v.z + q.w * q.v.y);
	result.m[2][1] = 2.0f * (q.v.y * q.v.z - q.w * q.v.x);
	result.m[2][2] = q.w * q.w - q.v.x * q.v.x - q.v.y * q.v.y + q.v.z * q.v.z;

	return result;
}
