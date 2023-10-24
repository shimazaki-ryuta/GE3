#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;
	Vector3& operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	};
};

/// <summary>
/// 直線
/// </summary>
struct Line {
	Vector3 origin;//!< 始点
	Vector3 diff;  //!< 終点への差分ベクトル
};
/// <summary>
/// 半直線
/// </summary>
struct Ray {
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};
/// <summary>
/// 線分
/// </summary>
struct Segment {
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};