#include "BaseScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Input/Input.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"

#include "imgui.h"
#include <typeinfo>

BaseScene::BaseScene(BaseScene::ID sceneID) :
#ifdef _DEBUG
	debugCamera_(std::make_unique<DebugCamera>()),
	isDebug_(false),
#endif // _DEBUG

	sceneManager_(nullptr),
	meshManager_(nullptr),
	audioManager_(nullptr),
	textureManager_(nullptr),
	frameInfo_(nullptr),
	input_(nullptr),
	stringOutPutManager_(nullptr),
	sceneID_(sceneID),
	camera_(std::make_unique<Camera>()),
	currentCamera_(camera_.get())
{}

void BaseScene::SceneInitialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;

	meshManager_ = MeshManager::GetInstance();

	audioManager_ = AudioManager::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	frameInfo_ = FrameInfo::GetInstance();

	stringOutPutManager_ = StringOutPutManager::GetInstance();

	input_ = Input::GetInstance();
}

void BaseScene::ChangeCamera()
{
#ifdef _DEBUG
	ImGui::Begin("SceneCamera");
	if (ImGui::Checkbox("debug", &isDebug_)) {
		currentCamera_ = isDebug_ ? debugCamera_.get() : camera_.get();
		if (isDebug_) {
			debugCamera_->pos = camera_->pos;
			debugCamera_->rotate = camera_->rotate;
			debugCamera_->scale = camera_->scale;
		}
	}
	ImGui::End();
#endif // _DEBUG
}

const Camera& BaseScene::GetCamera() const
{
	return *currentCamera_;
}
