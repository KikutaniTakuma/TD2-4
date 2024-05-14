#pragma once
#include "Camera/Camera.h"
#include "Camera/DebugCamera/DebugCamera.h"

#include "Drawers/DrawerManager.h"
#include "AudioManager/AudioManager.h"
#include "Input/Input.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"

#include "Utils/SafePtr/SafePtr.h"

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

	static constexpr int32_t kMaxScene = 4;

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
	
	DrawerManager* drawerManager_;

	AudioManager* audioManager_;

	FrameInfo* frameInfo_;

	Input* input_;

	StringOutPutManager* stringOutPutManager_;

	class UIEditor* uiEditor_;

	BaseScene::ID sceneID_;

private:
	std::unique_ptr<Camera> camera_;

	std::unique_ptr<Camera> texCamera_;

	

protected:
	Lamb::SafePtr<Camera> currentCamera_;

	Lamb::SafePtr<Camera> currentTexCamera_;

#ifdef _DEBUG
	std::unique_ptr<DebugCamera> debugCamera_;
	bool isDebug_;
#endif // _DEBUG
};