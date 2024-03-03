#pragma once

#include <unordered_map>
#include <string>
#include <type_traits>

#include "../GameObject.h"


class ComponentManager
{
	ComponentManager() = default;
	ComponentManager(const ComponentManager &) = delete;
	ComponentManager &operator=(const ComponentManager &) = delete;

	~ComponentManager() = default;
public:

	template <SoLib::IsBased<IComponent> T>
	void Register();



private:
	std::unordered_map<std::string, IComponent *(*)(GameObject *const object)> componentMap_;
};

template <SoLib::IsBased<IComponent> T>
inline void ComponentManager::Register() {
	componentMap_[typeid(T).name()] = T(GameObject *const);
}
