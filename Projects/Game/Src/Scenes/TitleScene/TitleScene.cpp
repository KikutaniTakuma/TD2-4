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
	currentCamera_->pos = Vector2{ 500.f, 300.f };

	tex_.reset( new Texture2D( "./Resources/Ball.png" ) );
	tex_->scale *= 30.0f;
	tex_->pos = Vector2{ 500.0f, 0.0f };

	sphere_.reset(new Texture2D("./Resources/Ball.png"));
	sphere_->scale *= 60.0f;
	sphere_->pos = Vector2{ 100.0f, 100.0f };
	sphere2_.reset(new Texture2D("./Resources/Ball.png"));
	sphere2_->scale *= 60.0f;
	sphere2_->pos = Vector2{ 1000.0f, 400.0f };

}

void TitleScene::Finalize(){
	itemManager_->Finalize();
	
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
	tex_->pos += moveVec.Normalize() * 100.0f * Lamb::DeltaTime();

	auto* const mouse = input_->GetMouse();

	if (mouse->LongPush(Mouse::Button::Left)) {
		tex_->pos = Vector3(mouse->GetPos()) * Mat4x4::MakeInverse(currentCamera_->GetViewOthographicsVp());
	}


	isCollision_ = Lamb::Collision::Capsule(
		sphere_->pos, sphere2_->pos, sphere_->scale.x * 0.5f,
		tex_->pos, tex_->scale.x * 0.5f
	);

	tex_->color = isCollision_ ? 0xff0000ff : 0xffffffff;

	tex_->Debug("テスト用サークル");
	tex_->Update();

	sphere_->Debug("Sphere");
	sphere_->Update();
	sphere2_->Debug("Sphere2");
	sphere2_->Update();

}

void TitleScene::Draw()
{
	tex_->Draw(currentCamera_->GetViewOthographics());

	sphere_->Draw(currentCamera_->GetViewOthographics());
	sphere2_->Draw(currentCamera_->GetViewOthographics());

	Line::Draw(sphere_->pos, sphere2_->pos, currentCamera_->GetViewOthographics(), 0xffffffff);

	Lamb::screenout << "Capsule Test" << Lamb::endline
		<< "Check : " << isCollision_;
}

void TitleScene::Debug(){


}
