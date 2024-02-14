#pragma once
#include <unordered_map>
#include <variant>
#include <string>
#include "Math/Vector3.h"

class GlobalVariables {
public:
	using Item = std::variant<int32_t, float, Vector3, std::string>;
	using Group = std::unordered_map<std::string, Item>;

public:
	GlobalVariables() = default;
	GlobalVariables(const std::string& fileNmae);
	GlobalVariables(const GlobalVariables&) = delete;
	GlobalVariables(GlobalVariables&&) noexcept = delete;
	~GlobalVariables() = default;

	GlobalVariables& operator=(const GlobalVariables&) = delete;
	GlobalVariables& operator=(GlobalVariables&&) noexcept = delete;

public:
	void CreateGroup(const std::string& groupName);

	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	void SetValue(const std::string& groupName, const std::string& key, float value);
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);
	void SetValue(const std::string& groupName, const std::string& key, const std::string& value);

	virtual void Update();

	void SaveFile(const std::string& groupName);
	virtual void LoadFile();
	virtual void LoadFile(const std::string& groupName);

	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	void AddItem(const std::string& groupName, const std::string& key, float value);
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);
	void AddItem(const std::string& groupName, const std::string& key, const std::string& value);

	int32_t GetIntValue(const std::string& groupName, const std::string& key) const;
	float GetFloatValue(const std::string& groupName, const std::string& key)const;
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key) const;
	std::string GetStringValue(const std::string& groupName, const std::string& key) const;

protected:
	std::unordered_map<std::string, Group> datas;

	bool isCreate;
	std::string groupNameAdd;

	std::string dataFileNmae;
};