#include "TitleScene.h"
#include "Game/Water/Water.h"
#include <cmath>
#include <numbers>
#include "Utils/EngineInfo/EngineInfo.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include "Game/CollisionManager/Capsule/Capsule.h"

TitleScene::TitleScene():
	BaseScene{BaseScene::ID::Title}
{
}

void TitleScene::Initialize()
{
	currentCamera_->pos = { 0.f, 0.f ,-1.0f };

	titleBGM_ = audioManager_->Load("./Resources/Sounds/BGM/title.mp3");
	beginGame_ = audioManager_->Load("./Resources/Sounds/SE/choice.mp3");
	titleBGM_->Start(0.1f, true);
	/*tex_.reset( new Texture2D( "./Resources/Ball.png" ) );
	tex_->scale *= 30.0f;
	tex_->pos = Vector2{ 500.0f, 0.0f };

	sphere_.reset(new Texture2D("./Resources/Ball.png"));
	sphere_->scale *= 60.0f;
	sphere_->pos = Vector2{ 100.0f, 100.0f };
	sphere2_.reset(new Texture2D("./Resources/Ball.png"));
	sphere2_->scale *= 60.0f;
	sphere2_->pos = Vector2{ 1000.0f, 400.0f };*/

}

void TitleScene::Finalize(){
	titleBGM_->Stop();

}

void TitleScene::Update()
{
	currentCamera_->Debug("カメラ");
	currentCamera_->Update();

	auto* const key = input_->GetKey();

	Vector3 moveVec;

	if (key->GetKey(DIK_W)) {
		moveVec.y = 1.0f;
	}
	if (key->GetKey(DIK_S)) {
		moveVec.y = -1.0f;
	}
	if (key->GetKey(DIK_D)) {
		moveVec.x = 1.0f;
	}
	if (key->GetKey(DIK_A)) {
		moveVec.x = -1.0f;
	}
	//tex_->pos += moveVec.Normalize() * 100.0f * Lamb::DeltaTime();

	auto* const mouse = input_->GetMouse();

	if (mouse->LongPush(Mouse::Button::Left)) {
		//tex_->pos = Vector3(mouse->GetPos()) * Mat4x4::MakeInverse(camera_->GetViewOthographicsVp());
	}
	if (key->Pushed(DIK_SPACE)) {
		beginGame_->Start(0.1f, false);
		sceneManager_->SceneChange(BaseScene::ID::StageSelect);
	}
	

	/*isCollision_ = Lamb::Collision::Capsule(
		sphere_->pos, sphere2_->pos, sphere_->scale.x * 0.5f,
		tex_->pos, tex_->scale.x * 0.5f
	);*/

	/*tex_->color = isCollision_ ? 0xff0000ff : 0xffffffff;

	tex_->Debug("テスト用サークル");
	tex_->Update();

	sphere_->Debug("Sphere");
	sphere_->Update();
	sphere2_->Debug("Sphere2");
	sphere2_->Update();*/

}

void TitleScene::Draw(){
	//tex_->Draw(camera_->GetViewOthographics());
	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());
	//sphere_->Draw(camera_->GetViewOthographics());
	//sphere2_->Draw(camera_->GetViewOthographics());

	//Line::Draw(sphere_->pos, sphere2_->pos, camera_->GetViewOthographics(), 0xffffffff);

	Lamb::screenout << "Capsule Test" << Lamb::endline
		<< "Check : " << isCollision_;

	UIEditor::GetInstance()->putDraw(currentCamera_->GetViewOthographics());
}

void TitleScene::Debug(){


}
