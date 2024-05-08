#include "Animation.h"
#include "Math/MatrixFunction.h"
#include "Math/VectorFunction.h"
#include "Math/QuaternionFunction.h"
#include "LoadModel.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cassert>

#include "CommonFiles/DirectXCommon.h"
ID3D12Device* Animation::sDevice = nullptr;
void  Animation::SetDevice(ID3D12Device* device)
{
	sDevice = device;
}

void Animation::Initialize() {
	isLoadingAnimation_ = false;
	time = 0.0f;
	playSpeed_ = 1.0f;
	matrixResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(Matrix4x4));

	matrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&matrixData_));
	*matrixData_ = MakeIdentity4x4();
};

void Animation::LoadAnimationFile(const std::string& directoryPath, const std::string& filename) {
	data_.reset(new AnimationData);
	//AnimationData animation;
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(),0);
	assert(scene->mNumAnimations != 0);

	aiAnimation* animationAssimp = scene->mAnimations[0];
	data_->duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = data_->nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		//position
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys;++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = {-keyAssimp.mValue.x,keyAssimp.mValue.y ,keyAssimp.mValue.z};//右手->左手
			nodeAnimation.translate.push_back(keyframe);
		}
		//rotate
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y ,-keyAssimp.mValue.z,keyAssimp.mValue.w };//右手->左手
			nodeAnimation.rotate.push_back(keyframe);
		}
		//scale
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y ,keyAssimp.mValue.z };
			nodeAnimation.scale.push_back(keyframe);
		}
	}
	isLoadingAnimation_ = true;
}
/*
Vector3 Animation::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time) {
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1;++index) {
		size_t nextIndex = index + 1;
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value,t);
		}
	}
	return (*keyframes.rbegin()).value;
}

Quaternion Animation::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	return (*keyframes.rbegin()).value;
}
*/
Matrix4x4 Animation::GetAnimationMatrix(const std::string& nodename) {
	NodeAnimation& nodeAnimation = data_->nodeAnimations[nodename];
	Vector3 translate = LoadModel::CalculateValue(nodeAnimation.translate,time);
	Quaternion rotate = LoadModel::CalculateValue(nodeAnimation.rotate, time);
	Vector3 scale = LoadModel::CalculateValue(nodeAnimation.scale, time);
	//return MakeScaleMatrix(scale) * MakeRotateMatrix(rotate) * MakeTranslateMatrix(translate);
	*matrixData_ = MakeScaleMatrix(scale) * MakeRotateMatrix(rotate) * MakeTranslateMatrix(translate);
	return MakeScaleMatrix(scale) * MakeRotateMatrix(rotate) * MakeTranslateMatrix(translate);
}

void Animation::Update() {
	if (isLoadingAnimation_) {
		time += 1.0f / 60.0f * playSpeed_;
		switch (playType_)
		{
		case LOOP:
			time = std::fmod(time, data_->duration);
			break;
		case ONE:
			time = std::min(time, data_->duration);
			break;
		default:
			time = std::fmod(time, data_->duration);
			break;
		}
	}
}