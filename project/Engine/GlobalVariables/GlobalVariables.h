#pragma once
#include <variant>
#include <map>
#include <stdint.h>
#include <string>
#include "Vector3.h"

//グローバル変数の管理、ファイル読み書きを行うクラス

class GlobalVariables {
public:
	
	using Item = std::variant<int32_t,float,Vector3>;
	using Group = std::map<std::string, Item>;

	//インスタンス取得
	static GlobalVariables* GetInstance();
	//グループ作製
	void CreateGroup(const std::string& groupName);

	//値セット
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	void SetValue(const std::string& groupName, const std::string& key, float value);
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

	//値追加
	void AddtValue(const std::string& groupName, const std::string& key, int32_t value);
	void AddItem(const std::string& groupName, const std::string& key, float value);
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);

	//値取得
	int32_t GetIntValue(const std::string& groupName, const std::string& key) ;
	float GetFloatValue(const std::string& groupName, const std::string& key) ;
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key) ;

	//更新
	void Update();

	//全ファイル読み込み
	void LoadFiles();
	//ファイル読み込み
	void LoadFile(const std::string& groupName);
	//ファイル保存
	void SaveFile(const std::string& groupName);

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables&) = delete;
	GlobalVariables operator=(const GlobalVariables&) = delete;

	std::map<std::string, Group> datas_;
	const std::string kDirectoryPath = "Resources/GlobalVariables/";
};
