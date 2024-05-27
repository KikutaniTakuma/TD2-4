#include "GameScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
#include "Engine/Graphics/PipelineObject/WaterPipeline/WaterPipeline.h"
#include "Game/Water/Water.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include"Scenes/SelectToGame/SelectToGame.h"
#include "Utils/Random/Random.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::TextureInitialize() {
	cloudType_[0] = "cloud1.png";
	cloudType_[1] = "cloud2.png";
	cloudType_[2] = "cloud3.png";

	cloudScale_[cloudType_[0]] = { 480,144 };
	cloudScale_[cloudType_[1]] = { 256,144 };
	cloudScale_[cloudType_[2]] = { 144,96 };

	for (uint32_t i = 0; i < kCloudNum_; i++) {
		clouds_[i] = std::make_unique<Tex2DState>();
		uint32_t num = Lamb::Random(0, 2);
		clouds_[i]->textureName = cloudType_[num];
		cloudsSpeed_[i] = Lamb::Random(3.0f, 1.0f);

		clouds_[i]->transform.translate = { Lamb::Random(300.0f,-1200.0f), Lamb::Random(-100.0f,300.0f) ,10.0f };
		clouds_[i]->transform.scale = cloudScale_[clouds_[i]->textureName];
		clouds_[i]->color = 0xffffffff;
		clouds_[i]->textureID = DrawerManager::GetInstance()->LoadTexture(("./Resources/UI/GameMain/" + clouds_[i]->textureName).c_str());
	}


	backGround_ = std::make_unique<Tex2DState>();
	backGround_->transform.scale = { 1280.0f,720.0f };
	backGround_->transform.translate = { 0.0f, 10.0f ,8.0f };
	backGround_->color = 0xffffffff;
	backGround_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/cutBackGround.png");

}


void GameScene::Initialize() {
	collisionManager_ = CollisionManager::GetInstance();

	currentCamera_->farClip = 3000.0f;
	currentCamera_->pos.y = 6.0f;
	currentCamera_->pos.z = -70.0f;
	currentCamera_->offset.z = -60.0f;
	currentCamera_->offset.y = 8.0f;
	currentCamera_->rotate.x = 0_deg;
	currentCamera_->drawScale = 0.036f;

	currentCamera_->scale.x = 0.5f;
	currentCamera_->scale.y = 0.5f;

	currentTexCamera_->farClip = 3000.0f;
	currentTexCamera_->pos.y = 10.0f;
	currentTexCamera_->pos.z = -70.0f;
	currentTexCamera_->offset.z = -60.0f;
	currentTexCamera_->offset.y = 8.0f;
	currentTexCamera_->rotate.x = 0_deg;
	currentTexCamera_->drawScale = 1.0f;

	GameManager::Finalize();
	gameManager_ = GameManager::GetInstance();
	gameManager_->Init();

	gameManager_->SetGameScene(this);

	//aabb_ = AABB::Create({ 0.0f,-0.5f,0.0f }, { 20.0f,1.0f,20.0f });

	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();


	TextureInitialize();

	gameBGM_ = audioManager_->Load("./Resources/Sounds/BGM/game.mp3");


	gameBGM_->Start(0.1f, true);

	shakePower_ = { 3.0f,3.0f };

	gameUIManager_ = std::make_unique<GameUIManager>();
	gameUIManager_->Init(gameManager_);

	pause_ = std::make_unique<PauseMenu>();
	pause_->Initialize();
	pause_->SetSceneManger(sceneManager_);
}

void GameScene::Finalize() {
	if (pause_) { pause_->Finalize(); }
	gameBGM_->Stop();

	//enemyManager_->Finalize();
}

void GameScene::Update() {
	// デルタタイム
	const float deltaTime = Lamb::DeltaTime();

	Lamb::SafePtr gamepad = Input::GetInstance()->GetGamepad();
	Lamb::SafePtr key = Input::GetInstance()->GetKey();
	if (gamepad->Pushed(Gamepad::Button::START) or key->Pushed(DIK_ESCAPE)) {
		pause_->isActive_ = not pause_->isActive_;
		if (pause_->isActive_) {
			gameBGM_->SetAudio(0.01f);
		}
		else {
			gameBGM_->SetAudio(0.1f);
		}
	}


	currentCamera_->Debug("カメラ");
	currentCamera_->Shake(1.0f);
	currentCamera_->Update();

	currentTexCamera_->Debug("UIカメラ");

	currentTexCamera_->Update();

	Debug();

	if (not pause_->isActive_) {
		//enemyManager_->Update();
		//enemyManager_->Debug();

		gameManager_->InputAction();
		gameManager_->Update(deltaTime);

		/*gameManager_->Debug("GameManager");*/

		collisionManager_->Update();
		collisionManager_->Debug();

		TextureUpdate();

		gameUIManager_->Update(deltaTime);

		if (input_->GetKey()->LongPush(DIK_RETURN) && input_->GetKey()->Pushed(DIK_BACKSPACE)) {
			Audio *cancel = audioManager_->Load("./Resources/Sounds/SE/cancel.mp3");

			gameBGM_->Stop();
			cancel->Start(0.1f, false);

			sceneManager_->SceneChange(BaseScene::ID::StageSelect);
		}
	}
	else {
		pause_->ActiveUpdate();
	}
}

void GameScene::TextureUpdate() {
	for (uint32_t i = 0; i < kCloudNum_; i++) {
		clouds_[i]->transform.translate.x += cloudsSpeed_[i];
		if (clouds_[i]->transform.translate.x > (700.0f) + (clouds_[i]->transform.scale.x)) {
			CloudReset(i);
		}
		clouds_[i]->transform.CalcMatrix();
	}


	//Vector3& uvTranslate = backGround_->uvTransform.translate;

	/*uvTranslate.x += 0.001f;
	uvTranslate.y += 0.0005f;
	if (1.0f <= uvTranslate.x) {
		uvTranslate.x -= 1.0f;
	}
	if (1.0f <= uvTranslate.y) {
		uvTranslate.y -= 1.0f;
	}*/

	backGround_->transform.CalcMatrix();
	backGround_->uvTransform.CalcMatrix();

}

void GameScene::CloudReset(const uint32_t cloudNumber) {
	clouds_[cloudNumber]->textureName = cloudType_[Lamb::Random(0, 2)];
	clouds_[cloudNumber]->transform.translate = { -640.0f - cloudScale_[clouds_[cloudNumber]->textureName].x,Lamb::Random(-100.0f,300.0f) };
	cloudsSpeed_[cloudNumber] = Lamb::Random(3.0f, 1.0f);
	clouds_[cloudNumber]->transform.scale = cloudScale_[clouds_[cloudNumber]->textureName];
	clouds_[cloudNumber]->color = 0xffffffff;
	clouds_[cloudNumber]->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/GameMain/" + clouds_[cloudNumber]->textureName);
}


void GameScene::Draw() {

	tex2D_->Draw(backGround_->transform.matWorld_, backGround_->uvTransform.matWorld_, currentTexCamera_->GetViewOthographics()
		, backGround_->textureID, backGround_->color, BlendType::kNormal);

	for (uint32_t i = 0; i < kCloudNum_; i++) {
		tex2D_->Draw(clouds_[i]->transform.matWorld_, Mat4x4::kIdentity, currentTexCamera_->GetViewOthographics()
			, clouds_[i]->textureID, clouds_[i]->color, BlendType::kNormal);
	}

	gameManager_->Draw(*currentCamera_);
	gameUIManager_->Draw(*currentTexCamera_);

	UIEditor::GetInstance()->Draw(currentTexCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());
	TextureDraw();
#ifdef _DEBUG

	UIEditor::GetInstance()->PutDraw(currentTexCamera_->GetViewOthographics());
#endif // _DEBUG

	pause_->ActiveDraw();
}

void GameScene::TextureDraw() {

}

void GameScene::Debug() {
#ifdef _DEBUG

#endif // _DEBUG
}
