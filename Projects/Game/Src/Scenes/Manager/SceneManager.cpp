#include "SceneManager.h"
#include "Engine/Engine.h"
#include "Input/Input.h"
#include "SceneFactory/SceneFactory.h"
#include "Engine/Graphics/ResourceManager/ResourceManager.h"

#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Utils/UtilsLib/UtilsLib.h"

#include "imgui.h"

#include <filesystem>
#include <fstream>
#include <format>

void SceneManager::Initialize(std::optional<BaseScene::ID> firstScene, std::optional<BaseScene::ID> finishID) {
	finishID_ = finishID;
	preSceneID_ = firstScene.value();


	fade_ = std::make_unique<Fade>();
	fadeCamera_.Update();

	frameInfo_ = FrameInfo::GetInstance();
	input_ = Input::GetInstance();

	SceneFactory* const sceneFactory = SceneFactory::GetInstance();

	scene_.reset(sceneFactory->CreateBaseScene(firstScene));
	scene_->SceneInitialize(this);
	scene_->Initialize();

	StringOutPutManager::GetInstance()->LoadFont("./Resources/Font/default.spritefont");


	load_.reset(new SceneLoad{});

	ResourceManager::GetInstance()->Enable();

#ifdef _DEBUG
	sceneName_[BaseScene::ID::Title] = "Title";
	sceneName_[BaseScene::ID::Game] = "Game";
#endif // _DEBUG
	sceneNum_;
	sceneNum_[BaseScene::ID::Title] = DIK_1;
	sceneNum_[BaseScene::ID::Game] = DIK_2;
}

void SceneManager::SceneChange(std::optional<BaseScene::ID> next) {
	if (next_ || fade_->InEnd()
		|| fade_->OutEnd() || fade_->IsActive()
		) 
	{
		return;
	}
	SceneFactory* const sceneFactory = SceneFactory::GetInstance();

	next_.reset(sceneFactory->CreateBaseScene(next));
	next_->SceneInitialize(this);


	fade_->OutStart();
}

void SceneManager::Update() {
	if (input_->GetGamepad()->PushAnyKey()) {
		isPad_ = true;
	}
	else if (input_->GetMouse()->PushAnyKey() || input_->GetKey()->PushAnyKey()) {
		isPad_ = false;
	}


	if (scene_ && !next_) {
		scene_->Update();
		Debug();
	}

	// フェードの更新処理
	fade_->Update();



	if (fade_->OutEnd()) {
		// ロード中の描画を開始
		load_->Start();

#pragma region シーン切り替え
		// 前のシーンのIDを保存
		preSceneID_ = scene_->GetID();

		// シーン終わり処理
		scene_->Finalize();
		// 次のシーンへ
		scene_.reset(next_.release());
		// 次のシーンを格納するものユニークポインタをリセット
		next_.reset();

		ResourceManager::GetInstance()->Unload();
#pragma endregion

#pragma region ロード中
		// シーンの初期化
		scene_->Initialize();
		// ロード中の描画を終了
		load_->Stop();
#pragma endregion

#pragma region その後の処理
		// フェードスタート
		fade_->InStart();

		// シーンの更新処理
		scene_->Update();
#pragma endregion
	}

}

void SceneManager::Draw() {
	if (scene_) {
		scene_->Draw();
	}

	fade_->Draw(fadeCamera_.GetViewOthographics());
}

bool SceneManager::IsEnd() const {
	if (!scene_) {
		return true;
	}

	return scene_->GetID() == finishID_ &&
		(input_->GetKey()->Pushed(DIK_ESCAPE) /*||
			input_->GetGamepad()->Pushed(Gamepad::Button::START)*/);
}

const Camera& SceneManager::GetCurrentSceneCamera() const
{
	return scene_->GetCamera();
}

BaseScene::ID SceneManager::GetCurrentSceneID() const
{
	return scene_->GetID();
}

BaseScene::ID SceneManager::GetPreSceneID() const
{
	return preSceneID_.value();
}

void SceneManager::Debug()
{
	if (input_->GetKey()->LongPush(DIK_SEMICOLON)) {
		for (auto& i : sceneNum_) {
			if (input_->GetKey()->Pushed(i.second)) {
				SceneChange(i.first);
				return;
			}
		}
	}
#ifdef _DEBUG
	ImGui::Begin("SceneManager");
	if (ImGui::TreeNode("シーン変更")) {
		for (auto& i : sceneName_) {
			if (ImGui::Button(i.second.c_str())) {
				SceneChange(i.first);
				break;
			}
		}
		ImGui::TreePop();
	}

	ImGui::Text((std::string("currentScene : ") + sceneName_[scene_->GetID()]).c_str());
	ImGui::Text((std::string("preScene : ") + sceneName_[preSceneID_.value()]).c_str());
	ImGui::End();
#endif // _DEBUG
}

void SceneManager::Finalize() {
	if (load_) {
		load_.reset();
	}


	fade_.reset();
	if (scene_) {
		scene_->Finalize();
	}
	scene_.reset();
	if (next_) {
		next_->Finalize();
	}
	next_.reset();

	ResourceManager::GetInstance()->Unenable();
}
