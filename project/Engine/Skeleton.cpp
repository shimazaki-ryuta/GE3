#include "Skeleton.h"
#include "MatrixFunction.h"
#include "QuaternionFunction.h"
void Skeleton::Initialize(Node& rootNode) {
	data_ = LoadModel::CreateSkelton(rootNode);
	for (Joint& joint : data_.joints) {
		WorldTransform wt;
		wt.Initialize();
		jointTransforms_.push_back(wt);
	}
	sphere_.reset(Model::CreateFromOBJ("uvSphere"));
}

void Skeleton::Update() {
	for (Joint& joint: data_.joints) {
		joint.localMatrix = MakeScaleMatrix(joint.transform.scale) * MakeRotateMatrix(joint.transform.rotate) * MakeTranslateMatrix(joint.transform.translate);
		if (joint.parent) {
			joint.skeltonSpaceMatrix = joint.localMatrix * data_.joints[*joint.parent].skeltonSpaceMatrix;
		} else {
			joint.skeltonSpaceMatrix = joint.localMatrix;
		}
	}
	for (uint32_t index = 0; index < data_.joints.size(); index++) {
		
		jointTransforms_[index].matWorld_ = data_.joints[index].skeltonSpaceMatrix;
	}
}

void Skeleton::ApplyAnimation(const AnimationData& animation, float animationTime) {
	for (Joint& joint : data_.joints) {
		//対象のJointのAnimationがあれば値を適応する
		if (auto it = animation.nodeAnimations.find(joint.name); it!=animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = LoadModel::CalculateValue(rootNodeAnimation.translate,animationTime);
			joint.transform.rotate = LoadModel::CalculateValue(rootNodeAnimation.rotate, animationTime);
			joint.transform.scale = LoadModel::CalculateValue(rootNodeAnimation.scale, animationTime);
		}
	}
}

void Skeleton::Draw(const WorldTransform& worldTransform,const ViewProjection& viewProjection) {
	for (WorldTransform& wt : jointTransforms_) {
		wt.matWorld_ = wt.matWorld_ * worldTransform.matWorld_;
		sphere_->Draw(wt,viewProjection);
	}
}