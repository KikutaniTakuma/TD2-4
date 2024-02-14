#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <bitset>
#include <optional>
#include "Fade/Fade.h"
#include "Input/Input.h"
#include "BaseScene/BaseScene.h"
#include "SceneLoad/SceneLoad.h"

class SceneManager final {
public:
	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	~SceneManager() = default;

	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

public:
	void Initialize(std::optional<BaseScene::ID> firstScene, std::optional<BaseScene::ID> finishID);
	void Finalize();

	void SceneChange(std::optional<BaseScene::ID> next);

	void Update();

	void Draw();

public:
	inline bool GetIsPad() const {
		return isPad_;
	}

	bool IsEnd() const;

	const class Camera& GetCurrentSceneCamera() const;

	BaseScene::ID GetCurrentSceneID() const;
	BaseScene::ID GetPreSceneID() const;


private:
	void Debug();


private:
	std::unique_ptr<BaseScene> scene_;
	std::unique_ptr<BaseScene> next_;

	std::unique_ptr<Fade> fade_;

	Camera fadeCamera_;

	class FrameInfo* frameInfo_ = nullptr;
	Input* input_ = nullptr;

	bool isPad_ = false;

	std::optional<BaseScene::ID> finishID_;
	std::optional<BaseScene::ID> preSceneID_;

	std::unique_ptr<SceneLoad> load_;

#ifdef _DEBUG
	std::unordered_map<BaseScene::ID, std::string> sceneName_;
#endif // _DEBUG
	std::unordered_map<BaseScene::ID, uint8_t> sceneNum_;
};