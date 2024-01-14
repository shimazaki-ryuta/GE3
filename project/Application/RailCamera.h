#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <vector>
class RailCamera 
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position, const Vector3& rotation);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	inline const WorldTransform& GetWorldTransform() { return worldTransForm_; };
	inline const ViewProjection& GetViewProjection() { return viewProjection_; };

	inline void SetControlPoints(std::vector<Vector3> controlPoints) {controlPoints_ = controlPoints;};

private:
	WorldTransform worldTransForm_;
	ViewProjection viewProjection_;
	std::vector<Vector3> controlPoints_;
	const size_t divisionRailCount = 1000;
	float t;
};
