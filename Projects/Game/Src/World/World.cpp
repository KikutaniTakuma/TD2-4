#include "World.h"
#include "Editor/ParticleEditor/ParticleEditor.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "Editor/ParticleEditor/ParticleEditor.h"
//#include "../Game/Water/Water.h"
//#include "../Game/Cloud/Cloud.h"

void World::Initialize() {
	// ウィンドウ初期化オプション
#ifdef _DEBUG
	initDesc_ = Framework::InitDesc{
		.windowName = "チェイン魔女娘",
		.windowSize = {1280.0f, 720.0f},
		.maxFps = 60.0f,
		.isFullesceen = false,
		.isMouseShow = true
	};
#else
	initDesc_ = Framework::InitDesc{
		.windowName = "チェイン魔女娘",
		.windowSize = {1280.0f, 720.0f},
		.maxFps = 60.0f,
		.isFullesceen = true,
		.isMouseShow = false
	};
#endif // _DEBUG

	// ロードのステータス
	SceneLoad::setting = SceneLoad::Desc{
		.fileName = "./Resources/Load.png",
		.animationNumber = 6,
		.animationSpeed = 0.1f
	};

	Framework::Initialize();

	//Water::Initialize();

	//Cloud::Initialize();

	StringOutPutManager::GetInstance()->LoadFont("./Resources/Font/mincho_size_32.spritefont");


	// シーンマネージャー初期化
	sceneManager_ = std::make_unique<SceneManager>();

	sceneManager_->Initialize(BaseScene::ID::Title, BaseScene::ID::Title);

	ParticleEditor::Initialize();
	particleEditor_ = ParticleEditor::GetInstance();
}

void World::Finalize() {
	//Cloud::Finalize();
	//Water::Finalize();

	if (sceneManager_) {
		sceneManager_->Finalize();
	}

	sceneManager_.reset();

	ParticleEditor::Finalize();
	GameManager::Finalize();

	Framework::Finalize();
}

void World::Update() {
	if (sceneManager_) {
		sceneManager_->Update();
		particleEditor_->Editor();
		isEnd_ = sceneManager_->IsEnd();
	}
	else {
		isEnd_ = true;
	}
}

void World::Draw() {
	if (sceneManager_) {
		sceneManager_->Draw();
		particleEditor_->Draw(sceneManager_->GetCurrentSceneCamera());
	}
	else {
		isEnd_ = true;
	}
}