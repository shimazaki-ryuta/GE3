#pragma once
#include "Quaternion.h"
#include "Matrix.h"

//クォータニオン計算関数群

Quaternion Add(const Quaternion& q, const Quaternion& r);
Quaternion Scaler(float scaler, const Quaternion& q);
Quaternion Multiply(const Quaternion& q, const Quaternion& r);
//乗法単位元
Quaternion IdentityQuaternion();
//共役クォータニオン
Quaternion Conjugate(const Quaternion& q);
//ノルム
float Norm(const Quaternion& q);
//正規化
Quaternion Normalize(const Quaternion& q);
//逆クォータニオン
Quaternion Inverse(const Quaternion& q);

//3次元ベクトルから回転クォータニオンを生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& vector, float angle);
//ベクトルを回転
Vector3 RotateVector(const Vector3& vector, const Quaternion& q);
//クォータニオンから回転行列を作る
Matrix4x4 MakeRotateMatrix(const Quaternion& q);
//内積
float Dot(const Quaternion& q0, const Quaternion& q1);
//球面線形補間
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1,float t);
Quaternion operator+(const Quaternion& q0, const Quaternion& q1);
Quaternion operator*(float scaler, const Quaternion& q);
Quaternion operator*(const Quaternion& q,float scaler);