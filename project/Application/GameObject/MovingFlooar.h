#pragma once
#include "Floor.h"

//動く床 Floorを継承する

class MovingFlooar : public Floor
{
public:
	//初期化
	void Initialize() override ;
	//更新
	void Update() override;
	
	//Setter
	void SetSwingWidth(float width) { swingWidth = width; };
private:
	Vector3 position_;
	Vector3 velocity_;
	float t;
	float direction_;

	//移動幅
	float swingWidth = 2.0f;
};

