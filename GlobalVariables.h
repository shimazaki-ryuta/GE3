#pragma once
#include <variant>
#include <map>
#include <stdint.h>
#include <string>
#include "Vector3.h"
class GlobalVariables {
public:
	/* struct Item {
		//項目
		std::variant<int32_t,float,Vector3> value;

	};*/
	using Item = std::variant<int32_t,float,Vector3>;
	/* struct Group {
		std::map<std::string, Item> items;
	};*/
	using Group = std::map<std::string, Item>;

	static GlobalVariables* GetInstance();
	void CreateGroup(const std::string& groupName);

	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	void SetValue(const std::string& groupName, const std::string& key, float value);
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

	void AddtValue(const std::string& groupName, const std::string& key, int32_t value);
	void AddItem(const std::string& groupName, const std::string& key, float value);
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);

	int32_t GetIntValue(const std::string& groupName, const std::string& key) ;
	float GetFloatValue(const std::string& groupName, const std::string& key) ;
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key) ;


	void Update();

	void LoadFiles();
	void LoadFile(const std::string& groupName);
	void SaveFile(const std::string& groupName);

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables&) = delete;
	GlobalVariables operator=(const GlobalVariables&) = delete;

	std::map<std::string, Group> datas_;
	const std::string kDirectoryPath = "Resources/GlobalVariables/";
};
