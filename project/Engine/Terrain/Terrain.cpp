#include "Terrain.h"
#include "MatrixFunction.h"
//ImGui
#include "../externals/imgui/imgui.h"
#include "../externals/imgui/imgui_impl_dx12.h"
#include "../externals/imgui/imgui_impl_win32.h"
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
	material_->paramater_.enableLighting = 1;
	material_->paramater_.disolveThreshold = 0.0f;
	material_->paramater_.disolveColor = Vector4{ 1.0f, 1.0f, 1.0f, 0.0f };
	material_->ApplyParamater();

	model_.reset(new Model);
	model_->CreateTerrain("Resources/Models", "PlayerHead.obj");
	//model_->CreateTerrain("Resources/TerrainTest", "TerrainTest.obj");
	//std::vector<VertexData>& vertices = model_->GetModelData().meshs.vertices;
	//vertices.clear();
}

void Terrain::SetTransformData(TerrainData& data) {
	worldtransform_.translation_ = data.object.transform.translate;
	worldtransform_.rotation_ = data.object.transform.rotate;
	worldtransform_.scale_ = data.object.transform.scale;
	worldtransform_.UpdateMatrix();
	worldtransform_.matWorld_ *= MakeAffineMatrix(deltaTransform_.scale, deltaTransform_.rotate, deltaTransform_.translate);
	
}

void Terrain::ResetMeshData(TerrainData& data) {
	std::vector<VertexData>& vertices = model_->GetModelData().meshs.vertices;
	vertices.clear();
	for (TerrainVerticesData& vData : data.verticesDatas) {
		VertexData* vertex;
		vertices.emplace_back();
		vertex = &vertices.back();
		vertex->position = { vData.position.x,vData.position.y,vData.position.z,1.0f };
		vertex->normal = vData.normal;
		vertex->texcoord = vData.uv;
	}
}

void Terrain::SetMeshData(TerrainData& data) {
	//総頂点数に満たすまで反映しない
	if (data.vertexNum_== -1 || data.verticesDatas.size() < data.vertexNum_) {
		return;
	}
	std::vector<VertexData>& vertices =  model_->GetModelData().meshs.vertices;
	vertices.clear();
	for (TerrainVerticesData & vData : data.verticesDatas) {
		VertexData* vertex;
		if (vertices.size()>vData.id && !vertices.empty()) {
			vertex = &vertices[vData.id];
		}
		else {
			vertices.emplace_back();
			vertex = &vertices.back();
		}
		vertex->position = { vData.position.x,vData.position.y,vData.position.z,1.0f };
		vertex->normal = vData.normal;
		vertex->texcoord = vData.uv;
		
		vData.id = -1;
	}
	data.verticesDatas.remove_if([](TerrainVerticesData& vData) {
		if (vData.id<0) {
			return true;
		}
		return false;
		});
	//data.verticesDatas.clear();
	data.vertexNum_ = -1;
}

void Terrain::Update() {
	static float st;
	ImGui::Begin("meshSync");
	ImGui::DragInt("shadeType", &(material_->paramater_.shadingType), 1, 0, 1);
	ImGui::DragFloat("Shiness", &(material_->paramater_.shininess), 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("EnvironmentSpecuraScale", &st, 0.01f, 0.0f, 2.0f);
	ImGui::DragFloat("Disolve", &(material_->paramater_.disolveThreshold), 0.001f, 0.0f, 1.0f);
	ImGui::ColorEdit3("DisolveColor", &(material_->paramater_.disolveColor.x));
	if (ImGui::Button("ClearVertex")) {
		std::vector<VertexData>& vertices = model_->GetModelData().meshs.vertices;
		vertices.clear();
	}
	ImGui::End();
	
	material_->paramater_.environmentCoefficient = st;
	material_->ApplyParamater();
	model_->SetMaterial(material_.get());

	worldtransform_.UpdateMatrix();
	worldtransform_.matWorld_ *= MakeAffineMatrix(deltaTransform_.scale, deltaTransform_.rotate, deltaTransform_.translate);
	
}

void Terrain::Draw(const ViewProjection& viewProjection) {
	
	model_->SetMaterial(material_.get());
	model_->Draw(worldtransform_, viewProjection);
}
