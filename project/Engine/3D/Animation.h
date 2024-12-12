#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include <d3d12.h>
#include <wrl.h>
#include "ModelStruct.h"

struct NodeAnimation {
	std::vector<KeyframeVector3> translate;
	std::vector<KeyframeQuaternion> rotate;
	std::vector<KeyframeVector3> scale;
};

struct AnimationData {
	float duration;
	std::map<std::string, NodeAnimation> nodeAnimations;
};

//アニメーションの再生を行うクラス

class Animation
{
public:

	//再生モード
	enum PlayType {
		LOOP,
		ONE
	};

	//初期化
	void Initialize();
	//gltf読み込み
	void LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	//更新
	void Update();

	//Getter/Setter
	void SetTime(float t) { time = t; };
	float GetTime() { return time; };
	void SetPlayType(PlayType type) { playType_ = type; };
	void SetPlaySpeed(float t) { playSpeed_ = t; };
	std::shared_ptr<AnimationData> GetAnimationData() { return data_; };

	//アニメーション適応行列を取得する
	Matrix4x4 GetAnimationMatrix(const std::string& nodename);


	//読み込み済みのアニメーションデータのポインタをセットする
	void SetAnimationData(std::shared_ptr<AnimationData> data) { 
		data_ = data;
		isLoadingAnimation_ = true;
	};

	// デバイス
	static ID3D12Device* sDevice;

	//DirectX12デバイスセット
	static void SetDevice(ID3D12Device* device);

	//DirectX用行列リソース取得
	ID3D12Resource* GetMatrixResource() {return matrixResource_.Get(); };

private:
	std::shared_ptr<AnimationData> data_;
	bool isLoadingAnimation_=false;
	float time = 0.0f;
	float playSpeed_=1.0f;//再生速度
	PlayType playType_ = LOOP;

	
	Microsoft::WRL::ComPtr<ID3D12Resource> matrixResource_;
	Matrix4x4* matrixData_ = nullptr;
};

