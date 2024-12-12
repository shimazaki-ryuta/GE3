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

//スキンアニメーション用スケルトン

class Skeleton {
public:
	//初期化
	void Initialize(Node &rootNode);
	//更新
	void Update();
	//アニメーション登録
	void ApplyAnimation(const AnimationData& animation, float animationTime);
	//Getter/Setter
	SkeletonData& GetSkeletonData() { return data_; };
	//デバッグ用描画
	void Draw(const WorldTransform& worldTransform,const ViewProjection& viewProjection);
private:
	SkeletonData data_;
	std::unique_ptr<Model> sphere_;
	std::vector<WorldTransform> jointTransforms_;
};