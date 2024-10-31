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
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
class Skeleton {
public:
	void Initialize(Node &rootNode);
	void Update();
	void ApplyAnimation(const AnimationData& animation, float animationTime);
	SkeletonData& GetSkeletonData() { return data_; };
	void Draw(const WorldTransform& worldTransform,const ViewProjection& viewProjection);
private:
	SkeletonData data_;
	std::unique_ptr<Model> sphere_;
	std::vector<WorldTransform> jointTransforms_;
};