#pragma once
#include "3D/WorldTransform.h"
#include "3D/ViewProjection.h"
#include "3D/Model.h"
#include "OBB.h"
#include <memory>

class Flooar
{
public:
	virtual void Initialize();
	virtual void Update();
	void Draw(const ViewProjection& viewProjection);
	OBB& GetOBB() { return obb_; };
	WorldTransform& GetWorldTransform() { return worldTransform_; };
	void SetOffset(const Vector3& offset) { 
		offset_ = offset;
		worldTransform_.translation_ = offset; 
	};
	void SetSize(const Vector3& size) { size_ = size; };

	void SetPerspectiveTextureHandle(uint32_t handle) { model_->SetPerspectivTextureHandle(handle); };
protected:
	WorldTransform worldTransform_;
	WorldTransform modelWorldTransform_;
	std::unique_ptr<Model> model_;
	OBB obb_;
	Vector3 size_;
	Vector3 offset_;
};

