#include "LoadModel.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"
#include <fstream>
#include <sstream>


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

ModelData LoadModel::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::string filePath = directoryPath + "/" + filename;
	
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());
	MeshData meshd;
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes;++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線要求
		assert(mesh->HasTextureCoords(0));//texcoord要求

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				VertexData vertex;
				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y, normal.z };
				vertex.texcoord = { texcoord.x,texcoord.y };
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				vertex.triangleCenter = {1.0f,1.0f,1.0f,1.0f};
				meshd.vertices.push_back(vertex);
				//meshs.push_back(mesh);
				//modelData.meshs.vertices.push_back(vertex);
			}
		}
		//modelData.vertexNum = 1;
		
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
	return modelData;
}

ModelData LoadModel::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::string filePath = directoryPath + "/" + filename;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());
	MeshData meshd;
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線要求
		assert(mesh->HasTextureCoords(0));//texcoord要求

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				VertexData vertex;
				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y, normal.z };
				vertex.texcoord = { texcoord.x,texcoord.y };
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				vertex.triangleCenter = { 1.0f,1.0f,1.0f,1.0f };
				meshd.vertices.push_back(vertex);
				//meshs.push_back(mesh);
				//modelData.meshs.vertices.push_back(vertex);
			}
		}
		//modelData.vertexNum = 1;

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
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	aiLocalMatrix.Transpose();
	for (int y = 0; y < 4;y++) {
		for (int x = 0; x < 4;x++) {
			result.localMatrix.m[y][x] = aiLocalMatrix[y][x];
		}
	}
	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren;++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}