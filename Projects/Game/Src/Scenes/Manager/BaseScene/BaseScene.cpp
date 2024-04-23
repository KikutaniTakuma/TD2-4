#include "BaseScene.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Input/Input.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"

BaseScene::BaseScene(BaseScene::ID sceneID) :
	drawerManager_(nullptr),
	sceneManager_(nullptr),
	audioManager_(nullptr),
	textureManager_(nullptr),
	frameInfo_(nullptr),
	input_(nullptr),
	stringOutPutManager_(nullptr),
	sceneID_(sceneID),
	camera_{ new Camera{} }
{}

void BaseScene::SceneInitialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;

	drawerManager_ = DrawerManager::GetInstance();

	audioManager_ = AudioManager::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	frameInfo_ = FrameInfo::GetInstance();

	stringOutPutManager_ = StringOutPutManager::GetInstance();

	input_ = Input::GetInstance();
}

const Camera& BaseScene::GetCamera() const
{
	return *camera_;
}
