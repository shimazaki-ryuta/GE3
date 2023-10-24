#pragma once
#include "flooar.h"
class MovingFlooar : public Flooar
{
public:
	void Initialize() override ;
	void Update() override;
	void SetSwingWidth(float width) { swingWidth = width; };
private:
	Vector3 position_;
	Vector3 velocity_;
	float t;
	float direction_;

	//移動幅
	float swingWidth = 2.0f;
};

