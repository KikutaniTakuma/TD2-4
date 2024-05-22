#pragma once
#include <unordered_map>
#include <variant>
#include <string>
#include "Math/Vector3.h"
#include "../SoLib/Containers/Singleton.h"

#include "../SoLib/Containers/VItem.h"

class GlobalVariables : public SoLib::Singleton<GlobalVariables> {
public:
	using Item = std::variant<int32_t, float, Vector2, Vector3, std::string>;
	using Group = std::unordered_map<std::string, Item>;

public:
	GlobalVariables() = default;
	GlobalVariables(const std::string &fileNmae);
	GlobalVariables(const GlobalVariables &) = delete;
	GlobalVariables(GlobalVariables &&) noexcept = delete;
	~GlobalVariables() = default;

	GlobalVariables &operator=(const GlobalVariables &) = delete;
	GlobalVariables &operator=(GlobalVariables &&) noexcept = delete;

public:
	void CreateGroup(const std::string &groupName);

	void SetValue(const std::string &groupName, const std::string &key, int32_t value);
	void SetValue(const std::string &groupName, const std::string &key, float value);
	void SetValue(const std::string &groupName, const std::string &key, const Vector2 &value);
	void SetValue(const std::string &groupName, const std::string &key, const Vector3 &value);
	void SetValue(const std::string &groupName, const std::string &key, const std::string &value);

	virtual void Update();

	void SaveFile(const std::string &groupName);
	virtual void LoadFile();
	virtual void LoadFile(const std::string &groupName);

	void AddItem(const std::string &groupName, const std::string &key, int32_t value);
	void AddItem(const std::string &groupName, const std::string &key, float value);
	void AddItem(const std::string &groupName, const std::string &key, const Vector2 &value);
	void AddItem(const std::string &groupName, const std::string &key, const Vector3 &value);
	void AddItem(const std::string &groupName, const std::string &key, const std::string &value);

	int32_t GetIntValue(const std::string &groupName, const std::string &key) const;
	float GetFloatValue(const std::string &groupName, const std::string &key)const;
	Vector3 GetVector3Value(const std::string &groupName, const std::string &key) const;
	std::string GetStringValue(const std::string &groupName, const std::string &key) const;

	Group *AddGroup(const std::string &group) {
		auto itr = datas.find(group);
		if (itr != datas.end()) { return &itr->second; }

		auto &g = datas[group];
		return &g;
	}
	Group *GetGroup(const std::string &group) {
		auto itr = datas.find(group);
		return itr != datas.end() ? &itr->second : nullptr;
	}
	const Group *GetGroup(const std::string &group) const {
		auto itr = datas.find(group);
		return itr != datas.end() ? &itr->second : nullptr;
	}

protected:
	std::unordered_map<std::string, Group> datas;

	bool isCreate;
	std::string groupNameAdd;

	std::string dataFileNmae;
};
template<typename T, typename Variant>
concept IsCanGetVariant = requires(Variant v) {
	{ std::get<T>(v) } -> std::convertible_to<T &>;
};

template <typename ITEM, SoLib::Text::ConstExprString V = ITEM::str_, IsCanGetVariant<GlobalVariables::Item> T = decltype(ITEM::item)>
	requires(std::same_as<ITEM, SoLib::VItem<V, T>>)
void operator>>(const GlobalVariables::Group &group, ITEM &item) {
	// グループから名前を検索
	GlobalVariables::Group::const_iterator itr = group.find(ITEM::c_str());

	// データが存在しない場合は終了
	if (itr == group.end()) { return; }

	// データを返す
	item = std::get<T>(itr->second);

}
template <typename ITEM, SoLib::Text::ConstExprString V = ITEM::str_, IsCanGetVariant<GlobalVariables::Item> T = decltype(ITEM::item)>
	requires(std::same_as<ITEM, SoLib::VItem<V, T>>)
void operator<<(GlobalVariables::Group &group, const ITEM &item) {

	// データを保存する
	group[ITEM::c_str()] = *item;

}

//template<typename T, typename... Ts>
//void operator>>(const GlobalVariables::Group &group, std::pair<T *, SoLib::VItemList<Ts...>> vItem) {
//	auto &&[item, list] = vItem;
//	for (uint32_t i = 0; i < list.size(); i++) {
//		std::visit([&group, item](const auto &arg) {
//			group >> item->*arg;
//			}, list[i]);
//	}
//}
//
//template<typename T, typename... Ts>
//void operator>>(GlobalVariables::Group &group, std::pair<T *, SoLib::VItemList<Ts...>> vItem) {
//	const auto &[item, list] = vItem;
//	for (uint32_t i = 0; i < list.size(); i++) {
//		std::visit([&group, item](const auto &arg) {
//			group << item->*arg;
//			}, list[i]);
//	}
//}

template<typename T, typename... Ts>
void LoadValue(const GlobalVariables::Group &group, T &item, const SoLib::VItemList<Ts...> &list) {
	for (uint32_t i = 0; i < list.size(); i++) {
		std::visit([&group, &item](const auto &arg) {
			group >> item.*arg;
			}, list[i]);
	}
}

template<typename T, typename... Ts>
void SaveValue(GlobalVariables::Group &group, const T &item, const SoLib::VItemList<Ts...> &list) {
	for (uint32_t i = 0; i < list.size(); i++) {
		std::visit([&group, &item](const auto &arg) {
			group << item.*arg;
			}, list[i]);
	}
}

template<typename... Ts>
void LoadValue(const GlobalVariables::Group &group, const SoLib::VItemList<Ts...> &list) {
	for (uint32_t i = 0; i < list.size(); i++) {
		std::visit([&group](const auto &arg) {
			group >> (*arg);
			}, list[i]);
	}
}

template<typename... Ts>
void SaveValue(GlobalVariables::Group &group, const SoLib::VItemList<Ts...> &list) {
	for (uint32_t i = 0; i < list.size(); i++) {
		std::visit([&group](const auto &arg) {
			group << (*arg);
			}, list[i]);
	}
}