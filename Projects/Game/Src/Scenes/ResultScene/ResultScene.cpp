#include "ResultScene.h"
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
#include "Utils/Random/Random.h"

ResultScene::ResultScene():
BaseScene{ BaseScene::ID::Result }
{
}

void ResultScene::Initialize(){
	currentCamera_->pos = { 0.f, 0.f ,-1.0f };

	/*tex_.reset(new Texture2D("./Resources/Ball.png"));
	tex_->scale *= 30.0f;
	tex_->pos = Vector2{ 300.0f, 0.0f };*/
}

void ResultScene::Finalize(){

}

void ResultScene::Update(){
	currentCamera_->Debug("カメラ");
	currentCamera_->Update();


	/*tex_->Debug("テスト用サークル");
	tex_->Update();*/
}

void ResultScene::Draw(){
	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());
	//tex_->Draw(currentCamera_->GetViewOthographics());

	UIEditor::GetInstance()->PutDraw(currentCamera_->GetViewOthographics());
}

void ResultScene::Debug(){

}
