#include "SceneLoad.h"
#include "Engine/Engine.h"
#include "./Camera/Camera.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Drawers/DrawerManager.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include <climits>

SceneLoad::Desc SceneLoad::setting = {};

SceneLoad::SceneLoad() :
	loadDrawThread_{},
	mtx_{},
	loadProc_{},
	loadTex_{},
	exit_(false),
	isLoad_(false),
	isWait_(false),
	tex2Danimator_(),
	renderContextManager_()
{
	renderContextManager_ = RenderContextManager::GetInstance();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	loadTex_ = drawerManager->GetTexture2D();

	tex2Danimator_ = std::make_unique<Tex2DAniamtor>();

	tex2Danimator_->SetStartPos(Vector2::kZero);
	tex2Danimator_->SetDuration(setting.animationSpeed);
	tex2Danimator_->SetAnimationNumber(setting.animationNumber);
	tex2Danimator_->SetLoopAnimation(true);
	textureID_ = drawerManager->LoadTexture(setting.fileName);

	std::unique_ptr<Camera> camera =  std::make_unique<Camera>();
	camera->pos.z = -1.0f;
	camera->Update();
	cameraMatrix_ = camera->GetViewOthographics();

	loadProc_ = [this]() {
		HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (SUCCEEDED(hr)) {
			Lamb::AddLog("CoInitializeEx succeeded");
		}
		else {
			throw Lamb::Error::Code<Engine>("CoInitializeEx failed", __func__);
		}

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

			tex2Danimator_->Update();

			loadTex_->Draw(
				Mat4x4::MakeAffin(Vector3(Lamb::ClientSize(), 1.0f), Vector3::kZero, Vector3::kZero),
				tex2Danimator_->GetUvMat4x4(),
				cameraMatrix_,
				textureID_,
				std::numeric_limits<uint32_t>::max(),
				BlendType::kNone
			);

			Engine::FrameEnd();

		}

		// COM 終了
		CoUninitialize();
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
		isLoad_ = true;

		Engine::FrameEnd();

		tex2Danimator_->Start();
		renderContextManager_->SetIsNowThreading(isLoad_);

		condition_.notify_all();
	}
}

void SceneLoad::Stop()
{
	if (isLoad_) {
		isLoad_ = false;
		while(!isWait_){

		}

		tex2Danimator_->Stop();
		renderContextManager_->SetIsNowThreading(isLoad_);
		Engine::FrameStart();
	}
}

void SceneLoad::CreateLoad()
{
	loadDrawThread_ = std::thread( loadProc_ );
}
