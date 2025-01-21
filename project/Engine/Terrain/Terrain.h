#pragma once
#include "Scene/SceneStructs.h"
#include "3D/Model.h"
#include <memory>

//MeshSync対応オブジェクトクラス

class Terrain
{
public:
	//初期化
	void Initialize(const GameObjectData& data);
	//更新
	void Update();
	//描画
	void Draw(const ViewProjection& viewProjection);
	
	//トランスフォーム情報セット
	void SetTransformData(TerrainData& data);

	//頂点情報モデルにセット
	void SetMeshData(TerrainData& data);

	//モデルの頂点情報をクリアし頂点数を0にする
	void ResetMeshData(TerrainData& data);
	~Terrain() {};

	//gettersetter
	void SetPerspectiveTextureHandle(uint32_t handle) { model_->SetPerspectivTextureHandle(handle); };

	VertexData* GetvertexData() { return model_->GetVertexData(); };

	//頂点数を設定する
	void SetVerticesNum(size_t num);

private:

	std::unique_ptr<Model> model_;
	WorldTransform worldtransform_;
	EulerTransform deltaTransform_;
	std::unique_ptr<Material> material_;
	std::string fileName = "";
	size_t verticesNum_ = 0;
};

