#pragma once
#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include "LoadModel.h"
#include "Animation.h"

class Skeleton {
public:
	void Initialize(Node &rootNode);
	void Update();
	void ApplyAnimation(const AnimationData& animation, float animationTime);
private:
	SkeletonData data_;
};