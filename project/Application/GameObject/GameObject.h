#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Colider/Collider.h"
#include <vector>
#include <memory>
#include <string>
#include "Scene/SceneStructs.h"
#include "Model.h"
#include "Material.h"
class GameObject
{
public:
	void Initialize(const GameObjectData& data);
	void Update() ;
	void Draw(const ViewProjection& viewProjection,std::map<std::string, std::unique_ptr<Model>>& modelList);

	//子ノード追加
	void AppendChildlen(std::unique_ptr<GameObject> child);
	void SetParent(WorldTransform* parent) { worldtransform_.parent_ = parent; };
	void SetParameter(const GameObjectData& data);
	std::vector<std::unique_ptr<GameObject>>* GetChildlen() { return &childlen_; };

	WorldTransform* GetWorldTransform() { return &worldtransform_; };

	void AppendColliderList(std::list<Collider*>& list);

	Collider* GetCollider() { return collider_.get(); };

private:
	WorldTransform worldtransform_;
	EulerTransform deltaTransform_;
	std::vector<std::unique_ptr<GameObject>> childlen_;
	std::unique_ptr<Collider> collider_;
	std::unique_ptr<Material> material_;
	std::string fileName = "";
};
