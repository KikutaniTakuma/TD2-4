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
	camera_->farClip = 3000.0f;
	camera_->pos.y = 30.0f;
	camera_->pos.z = -5.0f;
	camera_->offset.z = -60.0f;
	camera_->offset.y = 8.0f;

	water_ = Water::GetInstance();

	cloud_ = Cloud::GetInstance();

	skydome_ = std::make_unique<SkyDome>();
	skydome_->Initialize();
	skydome_->SetTexture(cloud_->GetTex());

	gameManager_ = GameManager::GetInstance();
	gameManager_->Init();

	aabb_ = AABB::Create(Vector3::kZero, Vector3::kIdentity);

	boxModel_ = std::make_unique<Model>("Resources/Cube.obj");
	boxModel_->pos = aabb_.GetCentor();
	boxModel_->scale = aabb_.GetRadius();
	boxModel_->Update();
}

void GameScene::Finalize() {

	// 実体の破棄
	GameManager::Finalize();
}

void GameScene::Update() {
	// デルタタイム
	const float deltaTime = Lamb::DeltaTime();

	camera_->Debug("カメラ");
	camera_->Update();

	water_->Update(camera_->GetPos());

	cloud_->Update();
	skydome_->Upadate();

	gameManager_->InputAction();
	gameManager_->Update(deltaTime);

	gameManager_->Debug("GameManager");

	if (aabb_.ImGuiDebug("AABB")) {
		boxModel_->pos = aabb_.GetCentor();
		boxModel_->scale = aabb_.GetRadius();
		boxModel_->Update();
	}

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::START)) {
		sceneManager_->SceneChange(BaseScene::ID::Title);
	}
}

void GameScene::Draw() {
	cloud_->Draw();
	skydome_->Draw(*camera_);

	water_->Draw(camera_->GetViewProjection());

	//boxModel_->Draw(camera_->GetViewProjection(), camera_->GetPos());

	gameManager_->Draw(*camera_);

	Lamb::screenout << "Water and cloud scene" << Lamb::endline
		<< "Press space to change ""Model scene""";
}