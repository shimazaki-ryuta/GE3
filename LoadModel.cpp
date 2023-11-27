#include "LoadModel.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
#include <fstream>
#include <sstream>
LoadModel::MaterialData LoadModel::LoadMaterialTemplateFile(const  std::string& directoryPath, const std::string& filename)
{
	LoadModel::MaterialData materialData;
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

LoadModel::ModelData LoadModel::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::vector<LoadModel::MeshData*> meshs;
	MeshData mesh;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;
	size_t vertexNum = 0;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifilter;
		std::istringstream s(line);
		s >> identifilter;
		if (identifilter == "o") {
			//mesh = new Model::MeshData;
			//meshs.push_back(mesh);
		}
		else if (identifilter == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifilter == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifilter == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (identifilter == "f") {
			LoadModel::VertexData triangle[3];
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
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = {position,texcoord,normal};
				//modelData.vertices.push_back(vertex);
				position.x *= -1.0f;
				normal.x *= -1.0f;
				texcoord.y = 1.0f - texcoord.y;
				triangle[faceVertex] = { position,texcoord,normal };
				vertexNum++;
			}
			mesh.vertices.push_back(triangle[2]);
			mesh.vertices.push_back(triangle[1]);
			mesh.vertices.push_back(triangle[0]);
		}
		else if (identifilter == "mtllib") {
			std::string materialFirename;
			s >> materialFirename;
			mesh.material = LoadModel::LoadMaterialTemplateFile(directoryPath, materialFirename);
		}
	}
	/*for (Model::MeshData* meshData : meshs)
	{
		modelData.meshs.push_back(*meshData);
		delete meshData;
	}*/
	modelData.meshs = mesh;
	//meshs.clear();
	modelData.vertexNum = vertexNum;
	return modelData;
}
