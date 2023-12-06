#pragma once
#include "Quaternion.h"

Quaternion Multiply(const Quaternion& q, const Quaternion& r);
//乗法単位元
Quaternion IdentityQuaternion();
//共役クォータニオン
Quaternion Conjugate(const Quaternion& q);
float Norm(const Quaternion& q);
Quaternion Normalize(const Quaternion& q);
Quaternion Inverse(const Quaternion& q);