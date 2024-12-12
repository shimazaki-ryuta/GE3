#pragma once
#include "3D/WorldTransform.h"
#include "3D/ViewProjection.h"
#include "Colider/Collider.h"
#include <vector>
#include <memory>
#include <string>
#include "Scene/SceneStructs.h"
#include "3D/Model.h"
#include "3D/Material.h"

//Blenderとリアルタイム対応させるためのクラス(今後全ての静的オブジェクトの基盤になる予定)

class GameObject
{
public:
	//初期化
	void Initialize(const GameObjectData& data);
	//更新
	void Update() ;
	//描画 modelList : シーンの持つ全モデルリスト
	void Draw(const ViewProjection& viewProjection,std::map<std::string, std::unique_ptr<Model>>& modelList);

	//子ノード追加
	void AppendChildlen(std::unique_ptr<GameObject> child);
	//Getter/Setter
	void SetParent(WorldTransform* parent) { worldtransform_.parent_ = parent; };
	void SetParameter(const GameObjectData& data);
	std::vector<std::unique_ptr<GameObject>>* GetChildlen() { return &childlen_; };
	Collider* GetCollider() { return collider_.get(); };
	WorldTransform* GetWorldTransform() { return &worldtransform_; };

	//自身(+子)の持つコライダーをリストに追加する
	void AppendColliderList(std::list<Collider*>& list);


private:
	WorldTransform worldtransform_;
	EulerTransform deltaTransform_;
	std::vector<std::unique_ptr<GameObject>> childlen_;
	std::unique_ptr<Collider> collider_;
	std::unique_ptr<Material> material_;
	std::string fileName = "";
};
