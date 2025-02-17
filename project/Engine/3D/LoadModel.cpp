#include "LoadModel.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"
#include "QuaternionFunction.h"
#include <fstream>
#include <sstream>
#include "Animation.h"
#include "GetDescriptorHandle.h"
#include "SRVManager.h"
size_t LoadModel::srvSkinClusterHandle = 0;
ID3D12Device* LoadModel::sDevice = nullptr;
ID3D12DescriptorHeap* LoadModel::sSrvDescriptorHeap=nullptr;

void LoadModel::SInitialize(ID3D12Device* device, ID3D12DescriptorHeap* descriptorHeap) {
	sDevice = device;
	sSrvDescriptorHeap = descriptorHeap;
}

MaterialData LoadModel::LoadMaterialTemplateFile(const  std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	while (std::getline(file, line)) {
		std::string identifilter;
		std::istringstream s(line);
		s >> identifilter;
		if (identifilter == "map_Kd")
		{
			std::string textureFirename;
			s >> textureFirename;
			materialData.textureFilePath = /*directoryPath + "/" + */ textureFirename;
			materialData.textureHandle = TextureManager::LoadTexture(materialData.textureFilePath);
		}
	}
	return materialData;
}
MaterialData LoadMaterialTemplateFile(const  std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	while (std::getline(file, line)) {
		std::string identifilter;
		std::istringstream s(line);
		s >> identifilter;
		if (identifilter == "map_Kd")
		{
			std::string textureFirename;
			s >> textureFirename;
			materialData.textureFilePath = /*directoryPath + "/" + */ textureFirename;
		}
	}
	return materialData;
}

ModelData LoadModel::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::vector<VertexData> verticeslistes_;//頂点座標から逆引きできるように
	std::string line;
	MeshData meshd;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifilter;
		std::istringstream s(line);
		s >> identifilter;

		if (identifilter == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			positions.push_back(position);
		}
		else if (identifilter == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifilter == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifilter == "f") {
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				
				
				meshd.indices.push_back(elementIndices[0] - 1);
				VertexData data;
				data.position = positions[elementIndices[0] - 1];
				data.texcoord = texcoords[elementIndices[1] - 1];
				data.normal = normals[elementIndices[2] - 1];
				verticeslistes_.push_back(data);
			}
	
		}
		else if (identifilter == "mtllib") {
			std::string materialFirename;
			s >> materialFirename;
			meshd.material = LoadMaterialTemplateFile(directoryPath, materialFirename);
		}
	}
	for (size_t i = 0; i < positions.size();i++) {
		for (VertexData& vData : verticeslistes_) {
				meshd.vertices.push_back(vData);
		}
	}
	modelData.meshs = meshd;
	modelData.vertexNum = meshd.vertices.size();
	return modelData;
}
ModelData LoadModel::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::string filePath = directoryPath + "/" + filename;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	assert(scene->HasMeshes());
	MeshData meshd;
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線要求
		assert(mesh->HasTextureCoords(0));//texcoord要求
		meshd.vertices.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			//uint32_t vertexIndex = face.mIndices[element];
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			VertexData vertex;
			vertex.position = { position.x,position.y,position.z,1.0f };
			vertex.normal = { normal.x,normal.y, normal.z };
			vertex.texcoord = { texcoord.x,texcoord.y };
			vertex.position.x *= -1.0f;
			vertex.normal.x *= -1.0f;
			//meshd.vertices.push_back(vertex);
			meshd.vertices[vertexIndex] = vertex;
			//meshs.push_back(mesh);
			//modelData.meshs.vertices.push_back(vertex);
		}
		//index解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				meshd.indices.push_back(vertexIndex);
			}
		}
		//skinCluster解析
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = meshd.skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindPoseMatrix = MakeScaleMatrix({ scale.x,scale.y,scale.z }) * MakeRotateMatrix({ rotate.x, -rotate.y,-rotate.z,rotate.w }) * MakeTranslateMatrix({-translate.x,translate.y,translate.z});
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);
			//weightの抽出
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights;++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId});
			}
		}

	}
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			std::string materialFirename = textureFilePath.C_Str();
			//meshd.material = LoadModel::LoadMaterialTemplateFile(directoryPath,materialFirename);
			meshd.material.textureFilePath = /*directoryPath + "/" + */ materialFirename;
			meshd.material.textureHandle = TextureManager::LoadTexture(meshd.material.textureFilePath);
		}
	}
	modelData.meshs = meshd;
	modelData.vertexNum = modelData.meshs.vertices.size();
	modelData.rootNode = ReadNode(scene->mRootNode);
	return modelData;
}

Node LoadModel::ReadNode(aiNode* node) {
	Node result;
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale,rotate,translate);
	result.transform.scale = {scale.x,scale.y,scale.z};
	result.transform.rotate = {rotate.x,-rotate.y ,-rotate.z ,rotate.w};
	result.transform.translate = {-translate.x,translate.y, translate.z};
	result.localMatrix = MakeScaleMatrix(result.transform.scale) * MakeRotateMatrix(result.transform.rotate) * MakeTranslateMatrix(result.transform.translate);

	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren;++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

SkeletonData LoadModel::CreateSkelton(const Node& rootNode) {
	SkeletonData skeleton;
	skeleton.root = CreateJoint(rootNode, {},skeleton.joints);

	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name,joint.index);
	}

	return skeleton;
}

int32_t LoadModel::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeltonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());
	joint.parent = parent;
	joints.push_back(joint);

	for (const Node& child : node.children) {
		int32_t childlenIndex = CreateJoint(child,joint.index,joints);
		joints[joint.index].children.push_back(childlenIndex);
	}

	return joint.index;
}

Vector3 LoadModel::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time) {
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	return (*keyframes.rbegin()).value;
}

Quaternion LoadModel::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	return (*keyframes.rbegin()).value;
}

SkinCluster LoadModel::CreateSkinCluster(const SkeletonData& skeleton, const ModelData& modelData) {
	SkinCluster skinCluster;
	//palette
	skinCluster.paletteResource = DirectXCommon::CreateBufferResource(sDevice,sizeof(WellForGPU) * skeleton.joints.size() );
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0,nullptr,reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = {mappedPalette,skeleton.joints.size()};
	
	//srvSkinClusterHandle++;
	//srv
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	//sDevice->CreateShaderResourceView(skinCluster.paletteResource.Get(),&paletteSrvDesc,skinCluster.palleteSrvHandle.first);
	srvSkinClusterHandle = SRVManager::GetInstance()->CreateSRV(skinCluster.paletteResource.Get(), &paletteSrvDesc);
	skinCluster.palleteSrvHandle.first = SRVManager::GetInstance()->GetCPUHandle(uint32_t(srvSkinClusterHandle));
	skinCluster.palleteSrvHandle.second = SRVManager::GetInstance()->GetGPUHandle(uint32_t(srvSkinClusterHandle));


	//influence用resource
	skinCluster.influenceResource = DirectXCommon::CreateBufferResource(sDevice, sizeof(VertexInfluence) * modelData.meshs.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0,nullptr,reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence,0,sizeof(VertexInfluence)* modelData.meshs.vertices.size());
	skinCluster.mappedInfluence = {mappedInfluence,modelData.meshs.vertices.size()};

	//influence用vbv
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence)*modelData.meshs.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	//inverseBindPoseの格納場所生成
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
	std::generate(skinCluster.inverseBindPoseMatrices.begin(),skinCluster.inverseBindPoseMatrices.end(),MakeIdentity4x4);
	
	//influenceを埋める
	for (const auto& jointWeight : modelData.meshs.skinClusterData){
		auto it = skeleton.jointMap.find(jointWeight.first);
		if (it == skeleton.jointMap.end()) {
			continue;
		}

		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
			for (uint32_t index = 0; index < kNumMaxInfluence;++index) {
				if (currentInfluence.weight[index] == 0.0f) {
					currentInfluence.weight[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}

	}
	return skinCluster;
}

void LoadModel::UpdateSkinCluster(SkinCluster& skinCluster, const SkeletonData& skeleton) {
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size();++jointIndex) {
		assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());
		skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix = skinCluster.inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeltonSpaceMatrix;
		skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Transpose(Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}