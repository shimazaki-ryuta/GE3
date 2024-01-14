#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "OBB.h"
#include <memory>

class Goal
{
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);

	OBB& GetOBB() { return obb_; };

private:

	WorldTransform worldTransform_;
	std::unique_ptr<Model> model_;
	uint32_t textureHandle_;
	OBB obb_;
	Vector3 size_;
	Vector3 offset_;
};

