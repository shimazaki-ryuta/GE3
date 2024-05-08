#include "Skeleton.h"
#include "MatrixFunction.h"
#include "QuaternionFunction.h"
void Skeleton::Initialize(Node& rootNode) {
	data_ = LoadModel::CreateSkelton(rootNode);
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