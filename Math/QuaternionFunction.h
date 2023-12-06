#pragma once
#include "Quaternion.h"
#include "Matrix.h"
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