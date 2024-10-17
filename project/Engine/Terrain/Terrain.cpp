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
	model_->CreateTerrain("Resources/TerrainTest", "TerrainTest.obj");

}

void Terrain::SetMeshData(TerrainData& data) {
	worldtransform_.translation_ = data.object.transform.translate;
	worldtransform_.rotation_ = data.object.transform.rotate;
	worldtransform_.scale_ = data.object.transform.scale;
	worldtransform_.UpdateMatrix();
	worldtransform_.matWorld_ *= MakeAffineMatrix(deltaTransform_.scale, deltaTransform_.rotate, deltaTransform_.translate);
	std::vector<VertexData>& vertices =  model_->GetModelData().meshs.vertices;
	//VertexData* vertices = model_->GetVertexData();
	//一旦古いデータ全部消して一から(後で変える)
	vertices.clear();
	for (TerrainVerticesData & vData : data.verticesDatas) {
		vertices.emplace_back();
		VertexData& vertex = vertices.back();
		vertex.position = { vData.position.x,vData.position.y,vData.position.z,1.0f };
		vertex.normal = vData.normal;
		vertex.texcoord = vData.uv;
		//vertices[vData.id].position = { vData.position.x,vData.position.y,vData.position.z,1.0f };
		//vertices[vData.id].normal = vData.normal;
		//vertices[vData.id].texcoord = vData.uv;
	}
}

void Terrain::Update() {
	worldtransform_.UpdateMatrix();
	worldtransform_.matWorld_ *= MakeAffineMatrix(deltaTransform_.scale, deltaTransform_.rotate, deltaTransform_.translate);
	
}

void Terrain::Draw(const ViewProjection& viewProjection) {
	
	model_->SetMaterial(material_.get());
	model_->DrawTerrain(worldtransform_, viewProjection);
}
