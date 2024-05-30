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

	objectiveBackGround_ = std::make_unique<Tex2DState>();
	objectiveBackGround_->transform.scale = { 1280.0f,720.0f };
	objectiveBackGround_->transform.translate = { 0.0f, 10.0f ,-40.0f };
	objectiveBackGround_->color = 0x000000aa;/*C:\Users\aoaomidori\Desktop\TD2-4\Solustion\Projects\Game\Resources\GameObjective*/
	objectiveBackGround_->textureID = TextureManager::GetInstance()->GetWhiteTex();

	objectiveFrame_ = std::make_unique<Tex2DState>();
	objectiveFrame_->transform.scale = { 768.0f,512.0f };
	objectiveFrame_->transform.translate = { 0.0f, 10.0f ,-41.0f };
	objectiveFrame_->color = 0xffffffff;/*C:\Users\aoaomidori\Desktop\TD2-4\Solustion\Projects\Game\Resources\GameObjective*/
	objectiveFrame_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/GameObjective/goalFramSet.png");

	for (uint32_t i = 0; i < 3; i++) {
		potNumberTexture_[i] = std::make_unique<Tex2DState>();
		potNumberTexture_[i]->transform.scale = { 80.0f,80.0f };
		potNumberTexture_[i]->transform.translate = { 70.0f * i,-300.0f ,-41.0f};
		potNumberTexture_[i]->uvTransform.scale = { 0.1f,1.0f };
		potNumberTexture_[i]->color = 0xffffffff;
		potNumberTexture_[i]->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/Timer/timeLimitNumber.png");

		timerNumberTexture_[i] = std::make_unique<Tex2DState>();
		timerNumberTexture_[i]->transform.scale = { 80.0f,80.0f };
		timerNumberTexture_[i]->transform.translate = { 70.0f * i,-227.0f ,-41.0f };
		timerNumberTexture_[i]->uvTransform.scale = { 0.1f,1.0f };
		timerNumberTexture_[i]->color = 0xffffffff;
		timerNumberTexture_[i]->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/Timer/timeLimitNumber.png");

	}

	timerState_ = std::make_unique<Tex2DState>();
	timerState_->color = 0xffffffff;
	timerState_->transform.scale = { 90.0f,102.0f };//136
	timerState_->transform.translate = { -170.0f, -43.0f ,-41.0f };
	timerState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Timer/timer.png");

	clockHandsState_ = std::make_unique<Tex2DState>();
	clockHandsState_->color = 0xffffffff;
	clockHandsState_->transform.scale = { 90.0f,102.0f };
	clockHandsState_->transform.translate = { -170.0f, -46.0f ,-41.0f };
	clockHandsState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Timer/timerNeedle.png");

	potState_ = std::make_unique<Tex2DState>();
	potState_->color = 0xffffffff;
	potState_->transform.scale = { 90.0f,102.0f };
	potState_->transform.translate = { -170.0f, -170.0f ,-41.0f };
	potState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/pot.png");

	clockNumberPos_ = { -55.0f,-43.0f };
	potNumberPos_ = { -55.0f,-166.0f };

	timerPos_ = { -170.0f, -43.0f ,-25.0f };
	clockHandsPos_ = { -170.0f, -46.0f ,-25.0f };
	potPos_ = { -170.0f, -170.0f ,-25.0f };

	numberDistance_ = 105.0f;

	ease_.Start(false, 1.5f, Easeing::InBack);

	easeCount_ = 90;

	isFadeOut_ = false;;
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
	audioManager_->Load("./Resources/Sounds/SE/damege.mp3");
	audioManager_->Load("./Resources/Sounds/SE/noSpace.mp3");



	shakePower_ = { 3.0f,3.0f };

	gameUIManager_ = std::make_unique<GameUIManager>();
	gameUIManager_->Init(gameManager_);

	pause_ = std::make_unique<PauseMenu>();
	pause_->Initialize();
	pause_->SetSceneManger(sceneManager_);

	isFirstLoadFlag_ = true;

	if (SelectToGame::GetInstance()->GetRetryFlug()) {
		isEndObjective_ = true;
	}
	else {
		isEndObjective_ = false;
	}
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

	

	Debug();

	if (not isEndObjective_ and  not isFirstLoadFlag_) {
		auto itemMax = gameManager_->GetClearItemCount();

		CalcUVPos(gameManager_->GetGameTimer()->GetDeltaTimer().GetGoalFlame(), timerNumberTexture_);
		CalcUVPos(static_cast<float>(itemMax), potNumberTexture_);

		for (uint32_t i = 0; i < 3; i++) {
			potNumberTexture_[i]->transform.translate = { objectiveFrame_->transform.translate.x + potNumberPos_.x + numberDistance_ * i,objectiveFrame_->transform.translate.y + potNumberPos_.y ,-41.0f };
			timerNumberTexture_[i]->transform.translate = { objectiveFrame_->transform.translate.x + clockNumberPos_.x + numberDistance_ * i,objectiveFrame_->transform.translate.y + clockNumberPos_.y ,-41.0f };

			timerNumberTexture_[i]->transform.CalcMatrix();
			timerNumberTexture_[i]->uvTransform.CalcMatrix();
			potNumberTexture_[i]->transform.CalcMatrix();
			potNumberTexture_[i]->uvTransform.CalcMatrix();
		}

		if (not ease_.GetIsActive() && not isFadeOut_) {
			easeCount_ -= 1;
		}

		if (easeCount_ == 0){
			ease_.Start(false, 1.5f, Easeing::OutQuint);
			isFadeOut_ = true;
			easeCount_ = -1;
		}
		ease_.Update();

		if (not isFadeOut_) {
			objectiveFrame_->transform.translate.y = ease_.Get(easePoint_.x, easePoint_.y);
		}
		else {
			objectiveFrame_->transform.translate.y = ease_.Get(easePoint_.y, -easePoint_.x);
		}

		if (isFadeOut_ && not ease_.GetIsActive()) {
			isEndObjective_ = true;
			gameBGM_->Start(0.1f, true);
		}

		timerState_->transform.translate = objectiveFrame_->transform.translate + timerPos_;
		clockHandsState_->transform.translate = objectiveFrame_->transform.translate + clockHandsPos_;
		potState_->transform.translate = objectiveFrame_->transform.translate + potPos_;

		timerState_->transform.CalcMatrix();
		clockHandsState_->transform.CalcMatrix();
		potState_->transform.CalcMatrix();

		objectiveBackGround_->transform.CalcMatrix();
		objectiveFrame_->transform.CalcMatrix();
		if (gamepad->Pushed(Gamepad::Button::A) || key->Pushed(DIK_SPACE)) {
			isEndObjective_ = true;
			gameBGM_->Start(0.1f, true);
		}
	}
	else {
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
				Audio* cancel = audioManager_->Load("./Resources/Sounds/SE/cancel.mp3");

				gameBGM_->Stop();
				cancel->Start(0.1f, false);

				sceneManager_->SceneChange(BaseScene::ID::StageSelect);
			}
		}
		else {
			pause_->ActiveUpdate();
		}
		isFirstLoadFlag_ = false;
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
	if (not isEndObjective_) {
		tex2D_->Draw(objectiveBackGround_->transform.matWorld_, Mat4x4::kIdentity, currentTexCamera_->GetViewOthographics()
			, objectiveBackGround_->textureID, objectiveBackGround_->color, BlendType::kNormal);

		tex2D_->Draw(objectiveFrame_->transform.matWorld_, Mat4x4::kIdentity, currentTexCamera_->GetViewOthographics()
			, objectiveFrame_->textureID, objectiveFrame_->color, BlendType::kNormal);

		for (size_t i = 0; i < 3; i++) {
			
			tex2D_->Draw(potNumberTexture_[i]->transform.matWorld_, potNumberTexture_[i]->uvTransform.matWorld_, currentTexCamera_->GetViewOthographics()
				, potNumberTexture_[i]->textureID, potNumberTexture_[i]->color, BlendType::kNormal);

			tex2D_->Draw(timerNumberTexture_[i]->transform.matWorld_, timerNumberTexture_[i]->uvTransform.matWorld_, currentTexCamera_->GetViewOthographics()
				, timerNumberTexture_[i]->textureID, timerNumberTexture_[i]->color, BlendType::kNormal);
		}

		tex2D_->Draw(timerState_->transform.matWorld_, Mat4x4::kIdentity, currentTexCamera_->GetViewOthographics()
			, timerState_->textureID, timerState_->color, BlendType::kNormal);


		tex2D_->Draw(clockHandsState_->transform.matWorld_, Mat4x4::kIdentity, currentTexCamera_->GetViewOthographics()
			, clockHandsState_->textureID, clockHandsState_->color, BlendType::kNormal);

		tex2D_->Draw(potState_->transform.matWorld_, Mat4x4::kIdentity, currentTexCamera_->GetViewOthographics()
			, potState_->textureID, potState_->color, BlendType::kNormal);
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
	ImGui::Begin("目的表示関連");
	ImGui::DragFloat3("目的表示自体の座標", objectiveFrame_->transform.translate.data(), 1.0f);

	ImGui::DragFloat3("時計本体の位置", timerPos_.data(), 1.0f);
	ImGui::DragFloat3("時計の針の位置", clockHandsPos_.data(), 1.0f);
	ImGui::DragFloat3("釜の位置", potPos_.data(), 1.0f);
	ImGui::DragFloat3("時計の数字の位置", clockNumberPos_.data(), 1.0f);
	ImGui::DragFloat3("釜の数字の位置", potNumberPos_.data(), 1.0f);
	ImGui::DragFloat("数字の間隔", &numberDistance_, 1.0f);

	ImGui::End();
#endif // _DEBUG
}

void GameScene::CalcUVPos(const float InGameData, std::array<std::unique_ptr<Tex2DState>, 3>& uvPos){

	texUVPos_[0] = static_cast<int32_t>(InGameData) % 10;
	texUVPos_[1] = static_cast<int32_t>(InGameData / 10.0f) % 10;
	texUVPos_[2] = static_cast<int32_t>(InGameData / 100.0f) % 10;

	uvPos[0]->uvTransform.translate.x = texUVPos_[2] * 0.1f;
	uvPos[1]->uvTransform.translate.x = texUVPos_[1] * 0.1f;
	uvPos[2]->uvTransform.translate.x = texUVPos_[0] * 0.1f;
}
