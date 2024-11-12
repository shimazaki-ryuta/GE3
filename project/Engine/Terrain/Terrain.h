#pragma once
#include "Scene/SceneStructs.h"
#include "3D/Model.h"
#include <memory>
class Terrain
{
public:

	void Initialize(const GameObjectData& data);
	void Update();
	void Draw(const ViewProjection& viewProjection);
	
	void SetTransformData(TerrainData& data);
	void SetMeshData(TerrainData& data);
	void ResetMeshData(TerrainData& data);
	~Terrain() {};

private:

	std::unique_ptr<Model> model_;
	WorldTransform worldtransform_;
	EulerTransform deltaTransform_;
	std::unique_ptr<Material> material_;
	std::string fileName = "";
};

