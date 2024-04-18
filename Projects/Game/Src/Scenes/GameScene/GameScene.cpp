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

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	collisionManager_ = CollisionManager::GetInstance();

	currentCamera_->farClip = 3000.0f;
	currentCamera_->pos.y = 10.0f;
	currentCamera_->pos.z = -70.0f;
	currentCamera_->offset.z = -60.0f;
	currentCamera_->offset.y = 8.0f;
	currentCamera_->rotate.x = 0_deg;

	/*water_ = Water::GetInstance();*/

	/*cloud_ = Cloud::GetInstance();

	skydome_ = std::make_unique<SkyDome>();
	skydome_->Initialize();
	skydome_->SetTexture(cloud_->GetTex());*/

	gameManager_ = GameManager::GetInstance();
	gameManager_->Init();

	player_ = std::make_unique<Player>();
	player_->Initialize();
}

void GameScene::Finalize() {

	// 実体の破棄
	GameManager::Finalize();
}

void GameScene::Update() {
	// デルタタイム
	const float deltaTime = Lamb::DeltaTime();

	currentCamera_->Debug("カメラ");
	
	currentCamera_->Update();

	/*water_->Update(currentCamera_->GetPos());*/

	/*cloud_->Update();
	skydome_->Upadate();*/
	player_->Debug();
	player_->Update();


#ifdef _DEBUG

	

#endif // _DEBUG
	gameManager_->InputAction();
	gameManager_->Update(deltaTime);

	/*gameManager_->Debug("GameManager");*/

	collisionManager_->Update();
	collisionManager_->Debug();

	//player_->AllTrade();

	if (input_->GetKey()->Pushed(DIK_RETURN) || input_->GetGamepad()->Pushed(Gamepad::Button::START)) {
		sceneManager_->SceneChange(BaseScene::ID::Title);
	}
}

void GameScene::Draw() {
	/*cloud_->Draw();
	skydome_->Draw(*currentCamera_);*/

	player_->Draw(*currentCamera_);

#ifdef _DEBUG

	

#endif // _DEBUG

	gameManager_->Draw(*currentCamera_);

	Lamb::screenout << "Water and cloud scene" << Lamb::endline
		<< "Press space to change ""Model scene""";
}