#include "SceneFactory.h"
#include "Scenes/GameScene/GameScene.h"
#include "Scenes/TitleScene/TitleScene.h"
#include "Scenes/SelectScene/SelectScene.h"
#include "Scenes/ResultScene/ResultScene.h"


SceneFactory::SceneFactory():
	createScene_{}
{
	CreateFunctions();
}

SceneFactory* const SceneFactory::GetInstance() {
	static SceneFactory instance{};
	return &instance;
}

BaseScene* SceneFactory::CreateBaseScene(std::optional<BaseScene::ID> createSceneID) {
	return createScene_[createSceneID]();
}

void SceneFactory::CreateFunctions() {
	createScene_[BaseScene::ID::Game] =
		[]()->BaseScene* {
		return new GameScene();
		};
	createScene_[BaseScene::ID::Title] =
		[]()->BaseScene* {
		return new TitleScene();
		};
	createScene_[BaseScene::ID::Result] =
		[]()->BaseScene* {
		return new ResultScene();
		};
	createScene_[BaseScene::ID::StageSelect] =
		[]()->BaseScene* {
		return new SelectScene();
		};
}