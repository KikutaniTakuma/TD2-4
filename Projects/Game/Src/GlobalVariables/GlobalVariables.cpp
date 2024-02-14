#include "GlobalVariables/GlobalVariables.h"
#include "imgui.h"
#include "json.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <Windows.h> 

GlobalVariables::GlobalVariables(const std::string& fileNmae) :
	datas(),
	isCreate(false),
	groupNameAdd(),
	dataFileNmae(fileNmae)
{}

void GlobalVariables::CreateGroup(const std::string& groupName) {
	datas[groupName];
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, int32_t value) {
	auto& gruop = datas[groupName];
	Item item = value;
	gruop[key] = value;
}
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, float value) {
	auto& gruop = datas[groupName];
	Item item = value;
	gruop[key] = value;
}
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const Vector3& value) {
	auto& gruop = datas[groupName];
	Item item = value;
	gruop[key] = value;
}
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const std::string& value) {
	auto& gruop = datas[groupName];
	Item item = value;
	gruop[key] = value;
}

void GlobalVariables::Update() {
#ifdef _DEBUG


	if (!ImGui::Begin(" Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) {
		return;
	}


	for (auto itrGroup = datas.begin(); itrGroup != datas.end(); itrGroup++) {
		const std::string& groupName = itrGroup->first;
		auto& group = itrGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) {
			continue;
		}
		for (auto& itemitr : group) {
			const std::string& itemName = itemitr.first;

			auto& item = itemitr.second;

			if (std::holds_alternative<int32_t>(item)) {
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}
			else if (std::holds_alternative<float>(item)) {
				float* ptr = std::get_if<float>(&item);
				ImGui::SliderFloat(itemName.c_str(), ptr, -10.0f, 10.0f);
			}
			else if (std::holds_alternative<Vector3>(item)) {
				Vector3* ptr = std::get_if<Vector3>(&item);
				ImGui::SliderFloat3(itemName.c_str(), &ptr->x, -10.0f, 10.0f);
			}
		}

		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
#endif // _DEBUG
}

void GlobalVariables::SaveFile(const std::string& groupName) {
	auto itrGroup = datas.find(groupName);
	assert(itrGroup != datas.end());

	nlohmann::json root;

	root = nlohmann::json::object();

	root[groupName] = nlohmann::json::object();

	for (auto itemItr = itrGroup->second.begin(); itemItr != itrGroup->second.end(); itemItr++) {
		const std::string& itemName = itemItr->first;

		auto& item = itemItr->second;

		if (std::holds_alternative<int32_t>(item)) {
			root[groupName][itemName] = std::get<int32_t>(item);
		}
		else if (std::holds_alternative<float>(item)) {
			root[groupName][itemName] = std::get<float>(item);
		}
		else if (std::holds_alternative<Vector3>(item)) {
			auto tmp = std::get<Vector3>(item);
			root[groupName][itemName] = nlohmann::json::array({ tmp.x, tmp.y, tmp.z });
		}
		else if (std::holds_alternative<std::string>(item)) {
			root[groupName][itemName] = std::get<std::string>(item);
		}
	}

	const std::filesystem::path kDirectoryPath = "./Datas/" + dataFileNmae;

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}

	std::string groupNameTmp;
	for (auto& i : groupName) {
		if (i == '\0') {
			break;
		}
		groupNameTmp += i;
	}
	auto filePath = (kDirectoryPath.string() + groupNameTmp) + std::string(".json");

	std::ofstream file(filePath);

	if (file.fail()) {
		assert(!"fileSaveFailed");
		return;
	}

	file << std::setw(4) << root << std::endl;

	file.close();
}

void GlobalVariables::LoadFile() {
	const std::filesystem::path kDirectoryPath = "./Datas/" + dataFileNmae;

	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}

	std::filesystem::directory_iterator dirItr(kDirectoryPath);
	for (const auto& entry : dirItr) {
		const auto& filePath = entry.path();

		auto extention = filePath.extension().string();
		if (extention.compare(".json") != 0) {
			continue;
		}

		LoadFile(filePath.stem().string());
	}

}

void GlobalVariables::LoadFile(const std::string& groupName) {
	const std::filesystem::path kDirectoryPath = "./Datas/" + dataFileNmae;
	std::string filePath = kDirectoryPath.string() + groupName + std::string(".json");

	std::ifstream file(filePath);

	if (file.fail()) {
		return;
	}

	nlohmann::json root;

	file >> root;

	file.close();

	nlohmann::json::iterator groupItr = root.find(groupName);

	assert(groupItr != root.end());

	for (auto itemItr = groupItr->begin(); itemItr != groupItr->end(); itemItr++) {
		const std::string& itemName = itemItr.key();

		if (itemItr->is_number_integer()) {
			int32_t value = itemItr->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		else if (itemItr->is_number_float()) {
			float value = itemItr->get<float>();
			SetValue(groupName, itemName, value);
		}
		else if (itemItr->is_array() && itemItr->size() == 3) {
			Vector3 value = { itemItr->at(0),itemItr->at(1),itemItr->at(2) };
			SetValue(groupName, itemName, value);
		}
		else if (itemItr->is_string()) {
			std::string value = itemItr->get<std::string>();
			SetValue(groupName, itemName, value);
		}

	}
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, int32_t value) {
	auto& gruop = datas[groupName];
	auto itemItr = gruop.find(key);
	if (itemItr == gruop.end()) {
		Item item = value;
		gruop[key] = value;
	}
}
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, float value) {
	auto& gruop = datas[groupName];
	auto itemItr = gruop.find(key);
	gruop[key] = value;
	if (itemItr == gruop.end()) {
		Item item = value;
		gruop[key] = value;
	}
}
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const Vector3& value) {
	auto& gruop = datas[groupName];
	auto itemItr = gruop.find(key);
	gruop[key] = value;
	if (itemItr == gruop.end()) {
		Item item = value;
		gruop[key] = value;
	}
}
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const std::string& value) {
	auto& gruop = datas[groupName];
	auto itemItr = gruop.find(key);
	gruop[key] = value;
	if (itemItr == gruop.end()) {
		Item item = value;
		gruop[key] = value;
	}
}

int32_t GlobalVariables::GetIntValue(const std::string& groupName, const std::string& key) const {
	auto dataItr = datas.find(groupName);
	assert(dataItr != datas.end());
	const Group& group = dataItr->second;
	auto itemItr = group.find(key);
	assert(itemItr != group.end());
	const Item& item = itemItr->second;
	if (std::holds_alternative<int32_t>(item)) {
		return std::get<int32_t>(item);
	}
	else {
		assert(!"GlobalVariables::GetIntValue() errar Don't have init32_t");
		return 0;
	}
}
float GlobalVariables::GetFloatValue(const std::string& groupName, const std::string& key)const {
	auto dataItr = datas.find(groupName);
	assert(dataItr != datas.end());
	const Group& group = dataItr->second;
	auto itemItr = group.find(key);
	assert(itemItr != group.end());
	const Item& item = itemItr->second;
	if (std::holds_alternative<float>(item)) {
		return std::get<float>(item);
	}
	else {
		assert(!"GlobalVariables::GetFloatValue() errar Don't have float");
		return 0.0f;
	}
}
Vector3 GlobalVariables::GetVector3Value(const std::string& groupName, const std::string& key) const {
	auto dataItr = datas.find(groupName);
	assert(dataItr != datas.end());
	const Group& group = dataItr->second;
	auto itemItr = group.find(key);
	assert(itemItr != group.end());
	const Item& item = itemItr->second;
	if (std::holds_alternative<Vector3>(item)) {
		return std::get<Vector3>(item);
	}
	else {
		assert(!"GlobalVariables::GetVector3Value() errar Don't have Vector3");

		return Vector3();
	}
}
std::string GlobalVariables::GetStringValue(const std::string& groupName, const std::string& key) const {
	auto dataItr = datas.find(groupName);
	assert(dataItr != datas.end());
	const Group& group = dataItr->second;
	auto itemItr = group.find(key);
	assert(itemItr != group.end());
	const Item& item = itemItr->second;
	if (std::holds_alternative<std::string>(item)) {
		return std::get<std::string>(item);
	}
	else {
		assert(!"GlobalVariables::GetStringValue() errar Don't have Vector3");

		return std::string();
	}
}