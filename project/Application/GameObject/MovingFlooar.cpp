#include "MovingFlooar.h"
#include "VectorFunction.h"
#include <algorithm>

//ImGui
#include "../externals/imgui/imgui.h"
#include "../externals/imgui/imgui_impl_dx12.h"
#include "../externals/imgui/imgui_impl_win32.h"


void MovingFlooar::Initialize() {
	Floor::Initialize();
	position_ = {0.0f,0.0f,0.0f};
	velocity_ = {0.0f,0.0f,1.0f};
	t = 0;
	direction_ = 1.0f;
	swingWidth = 2.0f;
}

void MovingFlooar::Update() {
	
	t += direction_/60.0f;
	if (t>1.0f || t<0.0f) {
		t = std::clamp(t,0.0f,1.0f);
		direction_ *= -1.0f;
	}
	position_.z =(t-0.5f)*2.0f * swingWidth;
	worldTransform_.translation_ = offset_ + position_;
	Floor::Update();
}