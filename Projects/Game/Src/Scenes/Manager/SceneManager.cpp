#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneFactory/SceneFactory.h"

#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"

#include "imgui.h"

#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Engine.h"
#include "Utils/EngineInfo/EngineInfo.h"

void SceneManager::Initialize(std::optional<BaseScene::ID> firstScene, std::optional<BaseScene::ID> finishID) {
	finishID_ = finishID;
	preSceneID_ = firstScene.value();


	fade_ = std::make_unique<Fade>();
	fadeCamera_.pos.z = -10.0f;
	fadeCamera_.Update();

	frameInfo_ = FrameInfo::GetInstance();
	input_ = Input::GetInstance();

	StringOutPutManager::GetInstance()->LoadFont("./Resources/Font/default.spritefont");
	load_ = std::make_unique<SceneLoad>();
	// テクスチャデータのアップロード
	UploadTextureData();

	SceneFactory* const sceneFactory = SceneFactory::GetInstance();

	// ロード中の描画を開始
	Engine::FrameStart();
	Vector2 clienetSize = Lamb::ClientSize();
	load_->Start();
#pragma region シーン切り替え
	scene_.reset(sceneFactory->CreateBaseScene(firstScene));
#pragma endregion

#pragma region ロード中
	scene_->SceneInitialize(this);
	// シーンの初期化
	scene_->Initialize();

	auto particle = std::make_unique<Particle>();
	particle->LoadSettingDirectory("Enemy-Magic");
	particle->LoadSettingDirectory("Block-Break");
	particle->LoadSettingDirectory("MagicHand");
	particle->LoadSettingDirectory("Magic");
	particle->LoadSettingDirectory("Smoke");
	particle->LoadSettingDirectory("Player-Damaged-Red");
	particle->LoadSettingDirectory("Player-Damaged-Purple");
	particle->LoadSettingDirectory("Player-Damaged-Yellow");
	particle->LoadSettingDirectory("Player-Damaged-Blue");
	particle->LoadSettingDirectory("Player-Damaged-Green");
	particle->LoadSettingDirectory("ResultExplosion");
	particle->LoadSettingDirectory("Bomb");
	particle.reset();

	uiEditor_ = UIEditor::GetInstance();
	uiEditor_->Initialize(this);
	uiEditor_->LoadFileAll();

	// ロード中の描画を終了
	load_->Stop();
#pragma endregion

#pragma region その後の処理
	// フェードスタート
	fade_->InStart();

	// シーンの更新処理
	uiEditor_->Update(scene_->GetID());
	scene_->Update();
#pragma endregion
	DrawerManager::GetInstance()->GetTexture2D()->Draw(
		Mat4x4::MakeScalar(clienetSize),
		Mat4x4::kIdentity,
		Mat4x4::MakeTranslate(Vector3::kZIdentity * -10.0f).Inverse() * Mat4x4::MakeOrthographic(clienetSize.x, clienetSize.y, 0.1f, 100.0f),
		TextureManager::GetInstance()->GetWhiteTex(),
		0xff,
		BlendType::kUnenableDepthNone
	);
	Engine::FrameEnd();

	// テクスチャデータのアップロード
	UploadTextureData();



#ifdef _DEBUG
	sceneName_[BaseScene::ID::Title] = "Title";
	sceneName_[BaseScene::ID::Game] = "Game";
	sceneName_[BaseScene::ID::StageSelect] = "Select";
	sceneName_[BaseScene::ID::Result] = "Result";
#endif // _DEBUG
	sceneNum_;
	sceneNum_[BaseScene::ID::Title] = DIK_1;
	sceneNum_[BaseScene::ID::Game] = DIK_2;
	sceneNum_[BaseScene::ID::StageSelect] = DIK_3;
	sceneNum_[BaseScene::ID::Result] = DIK_4;



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
	if (scene_){
		uiEditor_->Update(scene_->GetID());
	}

	if (scene_ && !next_) {
#ifdef _DEBUG
		scene_->ChangeCamera();
#endif // _DEBUG

		
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
		uiEditor_->Update(scene_->GetID());
		scene_->Update();
#pragma endregion
	}

	// テクスチャデータのアップロード
	UploadTextureData();
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

void SceneManager::UploadTextureData() {
	auto textureManager = TextureManager::GetInstance();
	// このフレームで画像読み込みが発生していたらTextureをvramに送る
	textureManager->UploadTextureData();
	// dramから解放
	textureManager->ReleaseIntermediateResource();
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
}
