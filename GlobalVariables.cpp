#include "GlobalVariables.h"
//#include "ImGuiManager.h"
//ImGui
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "CommonFiles/WinApp.h"
#include <cassert>
#include <json.hpp>
#include <filesystem>
#include <fstream>
using namespace nlohmann;
GlobalVariables* GlobalVariables::GetInstance() {
	static GlobalVariables instance;
	return &instance;
}

void GlobalVariables::CreateGroup(const std::string& groupName) {
	datas_[groupName];
}

void GlobalVariables::SetValue(
	const std::string& groupName,
	const std::string& key, int32_t value)
{
	Group& group = datas_[groupName];

	//新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	group[key] = newItem;
}
void GlobalVariables::SetValue(
	const std::string& groupName, const std::string& key, float value) {
	Group& group = datas_[groupName];

	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	group[key] = newItem;
}
void GlobalVariables::SetValue(
	const std::string& groupName, const std::string& key, const Vector3& value) {
	Group& group = datas_[groupName];

	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	group[key] = newItem;
}

void GlobalVariables::AddtValue(
    const std::string& groupName, const std::string& key, int32_t value) {
	Group& group = datas_[groupName];
	std::map<std::string, Item>::iterator itItem = group.find(key.c_str());
	if (itItem ==group.end())
	{
		SetValue(groupName,key,value);
	}
}
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, float value) {
	Group& group = datas_[groupName];
	std::map<std::string, Item>::iterator itItem = group.find(key.c_str());
	if (itItem == group.end()) {
		SetValue(groupName, key, value);
	}
}
void GlobalVariables::AddItem(
    const std::string& groupName, const std::string& key, const Vector3& value) {
	Group& group = datas_[groupName];
	std::map<std::string, Item>::iterator itItem = group.find(key.c_str());
	if (itItem == group.end()) {
		SetValue(groupName, key, value);
	}
}


void GlobalVariables::Update() {
	if (!ImGui::Begin("Groval Variables", nullptr, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) return;

	for (std::map<std::string, Group>::iterator itGroup = datas_.begin();
		itGroup != datas_.end(); ++itGroup)
	{
		const std::string& groupName = itGroup->first;

		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) continue;
		for (std::map<std::string, Item>::iterator itItem = group.begin();
			itItem != group.end(); ++itItem)
		{
			const std::string& itemName = itItem->first;
			Item& item = itItem->second;

			if (std::holds_alternative<int32_t>(item)) {
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGui::DragInt(itemName.c_str(), ptr);
			}
			else if (std::holds_alternative<float>(item)) {
				float* ptr = std::get_if<float>(&item);
				ImGui::DragFloat(itemName.c_str(), ptr,0.1f);
			}
			else if (std::holds_alternative<Vector3>(item)) {
				Vector3* ptr = std::get_if<Vector3>(&item);
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr),0.1f);
			}
		}

		//
		ImGui::Text("\n");
		if (ImGui::Button("Save"))
		{
			SaveFile(groupName);
			std::string message = std::format("{}.json saved.",groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables",0);
		}
		ImGui::EndMenu();
	}


	ImGui::EndMenuBar();
	ImGui::End();
}

void GlobalVariables::SaveFile(const std::string& groupName)
{
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	//未登録チェック	
	assert(itGroup !=datas_.end());

	json root;
	root = json::object();

	root[groupName] = json::object();

	for (std::map<std::string, Item>::iterator itItem = itGroup->second.begin();
	     itItem != itGroup->second.end(); ++itItem) {
		const std::string& itemName = itItem->first;

		Item& item = itItem->second;

		if (std::holds_alternative<int32_t>(item)) {
			root[groupName][itemName] = std::get<int32_t>(item);
		}
		else if (std::holds_alternative<float>(item)) {
			root[groupName][itemName] = std::get<float>(item);
		}
		else if (std::holds_alternative<Vector3>(item)) {
			Vector3 value = std::get<Vector3>(item);
			root[groupName][itemName] = json::array({value.x,value.y,value.z});
		}
	}

	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath))
	{
		std::filesystem::create_directory(kDirectoryPath);
	}

	std::string filePath = kDirectoryPath + groupName + ".json";
	std::ofstream ofs;
	ofs.open(filePath);

	if (ofs.fail())
	{
		std::string message = "Failed open data file for write";
		MessageBoxA(nullptr, message.c_str(), "GrobalVariables",0);
		assert(0);
		return;
	}

	ofs << std::setw(4) << root << std::endl;
	ofs.close();
}

void GlobalVariables::LoadFiles() {
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath);
	for (const std::filesystem::directory_entry& entry : dir_it)
	{
		const std::filesystem::path& filePath = entry.path();

		//拡張子取得
		std::string extension = filePath.extension().string();

		//.json以外はスキップ
		if (extension.compare(".json") != 0)
		{
			continue;
		}

		LoadFile(filePath.stem().string());
	}
}

void GlobalVariables::LoadFile(const std::string& groupName) {
	std::string filePath = kDirectoryPath + groupName + ".json";

	std::ifstream ifs;

	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "Failed open data file for read";
		MessageBoxA(nullptr, message.c_str(), "GrobalVariables", 0);
		assert(0);
		return;
	}

	json root;
	ifs >> root;
	ifs.close();

	json::iterator itGroup = root.find(groupName);

	//未登録チェック
	assert(itGroup != root.end());

	for (json::iterator itItem = itGroup->begin();itItem != itGroup->end();++itItem)
	{
		const std::string& itemName = itItem.key();

		if (itItem->is_number_integer())
		{
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName,itemName,value);
		}
		else if (itItem->is_number_float()) {
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		} 
		else if (itItem->is_array() && itItem->size() == 3) {
			Vector3 value = {itItem->at(0), itItem->at(1), itItem->at(2)};
			SetValue(groupName, itemName,value);
		}
	}
}

int32_t GlobalVariables::GetIntValue(const std::string& groupName, const std::string& key)  {
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	// 未登録チェック
	assert(itGroup != datas_.end());

	Group& group = datas_[groupName];
	std::map<std::string, Item>::iterator itItem = group.find(key.c_str());
	assert(itItem != group.end());
	return std::get<int32_t>(itItem->second);
}
float GlobalVariables::GetFloatValue(const std::string& groupName, const std::string& key)  {
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	// 未登録チェック
	assert(itGroup != datas_.end());

	Group& group = datas_[groupName];
	std::map<std::string, Item>::iterator itItem = group.find(key.c_str());
	assert(itItem != group.end());
	return std::get<float>(itItem->second);
}
Vector3 GlobalVariables::GetVector3Value(const std::string& groupName, const std::string& key)  {
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	// 未登録チェック
	assert(itGroup != datas_.end());

	Group& group = datas_[groupName];
	std::map<std::string, Item>::iterator itItem = group.find(key.c_str());
	assert(itItem != group.end());
	return std::get<Vector3>(itItem->second);
}
