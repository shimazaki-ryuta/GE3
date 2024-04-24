#pragma once
#include "Quaternion.h"
#include "Matrix.h"
Quaternion Add(const Quaternion& q, const Quaternion& r);
Quaternion Scaler(float scaler, const Quaternion& q);
Quaternion Multiply(const Quaternion& q, const Quaternion& r);
//乗法単位元
Quaternion IdentityQuaternion();
//共役クォータニオン
Quaternion Conjugate(const Quaternion& q);
float Norm(const Quaternion& q);
Quaternion Normalize(const Quaternion& q);
Quaternion Inverse(const Quaternion& q);

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& vector, float angle);
Vector3 RotateVector(const Vector3& vector, const Quaternion& q);
Matrix4x4 MakeRotateMatrix(const Quaternion& q);
float Dot(const Quaternion& q0, const Quaternion& q1);
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1,float t);
Quaternion operator+(const Quaternion& q0, const Quaternion& q1);
Quaternion operator*(float scaler, const Quaternion& q);
Quaternion operator*(const Quaternion& q,float scaler);