#pragma once
#include <optional>
#include <unordered_map>
#include <functional>
#include "Scenes/Manager/BaseScene/BaseScene.h"

class SceneFactory final {
private:
	SceneFactory();
	SceneFactory(const SceneFactory&) = delete;
	SceneFactory(SceneFactory&&) = delete;
	~SceneFactory() = default;

	SceneFactory& operator=(const SceneFactory&) = delete;
	SceneFactory& operator=(SceneFactory&&) = delete;

public:
	static SceneFactory* const GetInstance();

public:
	BaseScene* CreateBaseScene(std::optional<BaseScene::ID> createSceneID);

private:
	void CreateFunctions();

private:
	std::unordered_map<std::optional<BaseScene::ID>, std::function<BaseScene* (void)>> createScene_;
};