#pragma once
#include "Camera/Camera.h"
#include "Camera/DebugCamera/DebugCamera.h"
#include <memory>

class BaseScene {
	friend class SceneManager;

public:
	enum class ID {
		Result,
		Title,
		StageSelect,
		Game,
	};

	static constexpr int32_t maxScene_ = 4;

public:
	BaseScene(BaseScene::ID sceneID);
	BaseScene(const BaseScene&) = delete;
	BaseScene(BaseScene&&) = delete;
	virtual ~BaseScene() = default;

	BaseScene& operator=(const BaseScene&) = delete;
	BaseScene& operator=(BaseScene&&) = delete;

public:
	void SceneInitialize(class SceneManager* sceneManager);

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	virtual void Update() = 0;
	virtual void Draw() = 0;

	void ChangeCamera();

	inline BaseScene::ID GetID() const {
		return sceneID_;
	}

	const class Camera& GetCamera() const;

protected:
	class SceneManager* sceneManager_;

	class MeshManager* meshManager_;

	class AudioManager* audioManager_;

	class TextureManager* textureManager_;

	class FrameInfo* frameInfo_;

	class Input* input_;

	class StringOutPutManager* stringOutPutManager_;

	class UIEditor* uiEditor_;

	BaseScene::ID sceneID_;

private:
	std::unique_ptr<Camera> camera_;

protected:
	Camera* currentCamera_;

#ifdef _DEBUG
	std::unique_ptr<DebugCamera> debugCamera_;
	bool isDebug_;
#endif // _DEBUG
};