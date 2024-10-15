#include "Terrain.h"
#include "MatrixFunction.h"
void Terrain::Initialize(const GameObjectData& data) {
	worldtransform_.Initialize();
	worldtransform_.translation_ = data.transform.translate;
	worldtransform_.rotation_ = data.transform.rotate;
	worldtransform_.scale_ = data.transform.scale;
	worldtransform_.UpdateMatrix();
	deltaTransform_ = data.deltaTransform;
	worldtransform_.matWorld_ *= MakeAffineMatrix(deltaTransform_.scale, deltaTransform_.rotate, deltaTransform_.translate);
	fileName = data.fileName;
	
	material_.reset(new Material);
	material_->Initialize();
	material_->paramater_.enableLighting = 2;
	material_->paramater_.disolveThreshold = 0.0f;
	material_->paramater_.disolveColor = Vector4{ 1.0f, 1.0f, 1.0f, 0.0f };
	material_->ApplyParamater();

	model_.reset(new Model);
	model_->CreateTerrain("Resources/Head", "PlayerHead.obj");

}

void Terrain::SetMeshData(TerrainData& data) {
	worldtransform_.translation_ = data.object.transform.translate;
	worldtransform_.rotation_ = data.object.transform.rotate;
	worldtransform_.scale_ = data.object.transform.scale;
	worldtransform_.UpdateMatrix();
	worldtransform_.matWorld_ *= MakeAffineMatrix(deltaTransform_.scale, deltaTransform_.rotate, deltaTransform_.translate);

	ModelDataFromBlender* modeldata = model_->GetModelDataFromBlender();
	for (FromBlenderJsonObject& rData : data.positions) {
		modeldata->positions[rData.id] = { rData.value.x,rData.value.y,rData.value.z,1.0f };
	}
	//for (FromBlenderJsonObject& rData : data.normals) {
		//modeldata->normals[rData.id] = { rData.value.x,rData.value.y,rData.value.z};
	//}
	size_t index=0;
	for (BlenderIndex& rData : data.indices) {
		modeldata->indices[index++] = { rData.position,rData.texcoord,rData.normal };
	}
}

void Terrain::Update() {
	worldtransform_.UpdateMatrix();
	worldtransform_.matWorld_ *= MakeAffineMatrix(deltaTransform_.scale, deltaTransform_.rotate, deltaTransform_.translate);
	
}

void Terrain::Draw(const ViewProjection& viewProjection) {
	
	model_->SetMaterial(material_.get());
	model_->DrawMeshShink(worldtransform_, viewProjection);
}
