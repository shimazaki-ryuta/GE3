#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Colider/Collider.h"
#include <vector>
#include <memory>
#include <string>
#include "Scene/SceneStructs.h"
#include "Model.h"
class GameObject
{
public:
	void Initialize(const GameObjectData& data);
	void Update() {};
	void Draw(const ViewProjection& viewProjection,std::map<std::string, std::unique_ptr<Model>>& modelList);

	//子ノード追加
	void AppendChildlen(std::unique_ptr<GameObject> child);
private:
	WorldTransform worldtransform_;
	
	std::vector<std::unique_ptr<GameObject>> childlen_;

	std::string fileName = "";
};
