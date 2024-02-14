#include "SceneLoad.h"
#include "Engine/Engine.h"
#include "./Camera/Camera.h"
#include "Utils/EngineInfo/EngineInfo.h"

SceneLoad::Desc SceneLoad::setting = {};

SceneLoad::SceneLoad() :
	loadDrawThread_{},
	mtx_{},
	loadProc_{},
	loadTex_{},
	exit_{ false },
	isLoad_{ false },
	isWait_{false}
{
	loadTex_.reset(new Texture2D{ setting.fileName });

	loadTex_->scale = Lamb::ClientSize();
	loadTex_->uvSize.x = 1.0f / static_cast<float>(setting.animationNumber);
	loadTex_->uvPibotSpd = 1.0f / static_cast<float>(setting.animationNumber);

	std::unique_ptr<Camera> camera{ new Camera{} };
	camera->Update();
	cameraMatrix_ = camera->GetViewOthographics();

	loadProc_ = [this]() {
		std::unique_lock<std::mutex> uniqueLock(mtx_);


		while (!exit_) {
			if (!isLoad_) {
				isWait_ = true;
				condition_.wait(uniqueLock, [this]() { return isLoad_ || exit_; });
			}
			if (exit_) {
				break;
			}
			isWait_ = false;
			Engine::FrameStart();

			loadTex_->Animation(
				static_cast<size_t>(setting.animationSpeed),
				true,
				0.0f,
				static_cast<float>(setting.animationNumber)
			);

			loadTex_->Update();

			loadTex_->Draw(cameraMatrix_);

			Engine::FrameEnd();

		}
	};

	CreateLoad();
}

SceneLoad::~SceneLoad()
{
	exit_ = true;
	condition_.notify_all();
	if (loadDrawThread_.joinable()) {
		loadDrawThread_.join();
	}
}

void SceneLoad::Start()
{
	if (!isLoad_) {
		loadTex_->AnimationStart();
		isLoad_ = true;

		loadTex_->Update();
		loadTex_->Draw(cameraMatrix_);
		Engine::FrameEnd();

		condition_.notify_all();
	}
}

void SceneLoad::Stop()
{
	if (isLoad_) {
		isLoad_ = false;
		while(!isWait_){

		}
		Engine::FrameStart();
	}
}

void SceneLoad::CreateLoad()
{
	loadDrawThread_ = std::thread{ loadProc_ };
}
