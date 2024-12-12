#pragma once
#include <string>
#include <stdint.h>
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include "ModelStruct.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3d12.h>

//モデル関係読み込み用クラス(静的初期化情報を共有するためのクラス)

class LoadModel
{
public:
	//静的初期化処理
	static void SInitialize(ID3D12Device* device, ID3D12DescriptorHeap* descriptorHeap);

	//obj読み込み(blender連携用手動展開版)
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	//モデルファイル読み込み
	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	//mtlファイル読み込み
	static MaterialData LoadMaterialTemplateFile(const  std::string& directoryPath, const std::string& filename);
	//ノード読み込み
	static Node ReadNode(aiNode* node);
	//スケルトン生成
	static SkeletonData CreateSkelton(const Node& rootNode);
	//ジョイント生成
	static int32_t CreateJoint(const Node& node,const std::optional<int32_t>& parent,std::vector<Joint>& joints);

	//アニメーション用計算処理
	static Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	static Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	//スキンクラスタ生成
	static SkinCluster CreateSkinCluster(const SkeletonData& skeleton, const ModelData& modelData);

	//スキンクラスタ更新
	static void UpdateSkinCluster(SkinCluster& skinCluster,const SkeletonData& skeleton);

	static const size_t srvSkinClusterStart = 768;
	static size_t srvSkinClusterHandle;
	// デバイス
	static ID3D12Device* sDevice;
	static ID3D12DescriptorHeap* sSrvDescriptorHeap;
};

