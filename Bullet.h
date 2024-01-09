#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Colider/Sphere.h"
class Bullet
{
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);

	void SetVelocity(const Vector3& vec) { velocity_ = vec; };
	bool GetIsDead() { return isDead_; };
	void SetIsDead(bool is) { isDead_ = is; };
	Sphere& GetSphere() { return sphere_; };
	void SetModel(Model* model) { model_ = model; };
	void SetPosition(const Vector3& pos) { worldTransform_.translation_ = pos; };
private:
	WorldTransform worldTransform_;
	Model* model_;
	Sphere sphere_;
	Vector3 velocity_;
	bool isDead_;
	int aliveTime_;
};

