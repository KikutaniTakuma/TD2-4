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

void TitleScene::Initialize(){
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

	moveSpeeds_.resize(3);

	moveSpeeds_[0] = 0.01f;
	moveSpeeds_[1] = 0.01f;
	moveSpeeds_[2] = 0.01f;

	currentCamera_->pos = { 0.f, 0.f ,-1.0f };

	titleBGM_ = audioManager_->Load("./Resources/Sounds/BGM/title.mp3");
	beginGame_ = audioManager_->Load("./Resources/Sounds/SE/choice.mp3");
	titleBGM_->Start(0.1f, true);

	for (int i = 0; i < 4; i++) {
		std::unique_ptr<Tex2DState> setTex_ = std::make_unique<Tex2DState>();
		setTex_->color = 0x000000ff;
		setTex_->transform.scale = { 10.0f, 10.0f };
		setTex_->transform.translate = { 10.0f, 10.0f };
		setTex_->uvTrnasform.scale = { 0.0f, 0.0f };
		setTex_->uvTrnasform.translate = { 0.0f, 0.0f };
		setTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Ball.png");
		setTex_->textureFullPath = "./Resources/Ball.png";
		setTex_->textureName = "Ball";
		spheres_.emplace_back(std::move(setTex_));

		ControlPoints_.push_back(controlPoint_[i]);
	}

	runTexture_= std::make_unique<Tex2DState>();
	runTexture_->color = 0x000000ff;
	runTexture_->transform.scale = { 50.0f, 50.0f };
	runTexture_->transform.translate = { 10.0f, 10.0f };
	runTexture_->uvTrnasform.scale = { 0.0f, 0.0f };
	runTexture_->uvTrnasform.translate = { 0.0f, 0.0f };
	runTexture_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/enemy_popEffect.png");
	runTexture_->textureFullPath = "./Resources/enemy_popEffect.png";
	runTexture_->textureName = "enemy_popEffect";

	for (size_t i = 0; i < (ControlPoints_.size() - 1) * divisionNumber_; i++) {
		lines_.push_back(std::make_unique<Line>());
	}

	LastLinePass_ = static_cast<int>(ControlPoints_.size()) - 2;

	mode_ = First;

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

	SetControlPoint();

	for (size_t i = 0; i < spheres_.size(); i++){
		spheres_[i]->transform.translate = ControlPoints_[i];
		spheres_[i]->transform.CalcMatrix();
	}
	MoveTexture();


	runTexture_->transform.CalcMatrix();
	
	Debug();
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
	drawCount_ = 0;

	//tex_->Draw(camera_->GetViewOthographics());
	//sphere_->Draw(camera_->GetViewOthographics());
	//sphere2_->Draw(camera_->GetViewOthographics());
	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());

	//for (size_t j = 0; j < spheres_.size(); j++) {
	//	tex2D_->Draw(spheres_[j]->transform.matWorld_, Mat4x4::kIdentity, currentCamera_->GetViewOthographics()
	//		, spheres_[j]->textureID, spheres_[j]->color, BlendType::kNormal);
	//}

	//tex2D_->Draw(runTexture_->transform.matWorld_, Mat4x4::kIdentity, currentCamera_->GetViewOthographics()
	//	, runTexture_->textureID, runTexture_->color, BlendType::kNormal);

	//DrawCatmullRom(ControlPoints_[0], ControlPoints_[0], ControlPoints_[1], ControlPoints_[2], currentCamera_->GetViewOthographics());
	//for (uint32_t i = 1; i < LastLinePass_; i++) {
	//	DrawCatmullRom(ControlPoints_[i - 1], ControlPoints_[i], ControlPoints_[i + 1], ControlPoints_[i + 2], currentCamera_->GetViewOthographics());
	//}

	//DrawCatmullRom(ControlPoints_[LastLinePass_ - 1], ControlPoints_[LastLinePass_], ControlPoints_[LastLinePass_ + 1], ControlPoints_[LastLinePass_ + 1], currentCamera_->GetViewOthographics());

	//DrawCatmullRom(ControlPoints_[0], ControlPoints_[1], ControlPoints_[2], ControlPoints_[3], currentCamera_->GetViewOthographics());
	//DrawCatmullRom(ControlPoints_[1], ControlPoints_[2], ControlPoints_[3], ControlPoints_[3], currentCamera_->GetViewOthographics());

	//Line::Draw(sphere_->pos, sphere2_->pos, camera_->GetViewOthographics(), 0xffffffff);

	Lamb::screenout << "Capsule Test" << Lamb::endline
		<< "Check : " << isCollision_;

	UIEditor::GetInstance()->PutDraw(currentCamera_->GetViewOthographics());
}

void TitleScene::DrawCatmullRom(
	const Vector2& controlPoint0, const Vector2& controlPoint1, const Vector2& controlPoint2, 
	const Vector2& controlPoint3, const Mat4x4& viewProjectionMatrix){
	//分割数
	const uint32_t divisionNumber = 8;
	//曲線の変数
	Vector2 CatmullRom[divisionNumber + 1] = {};
	float t = 0.0f;
	float lengthNum = 0.0f;



	for (uint32_t i = 0; i < divisionNumber + 1; i++) {
		t = i / static_cast<float>(divisionNumber);

		Vector2 p = Vector2::CatmullRom(controlPoint0, controlPoint1, controlPoint2, controlPoint3, t);

		CatmullRom[i] = p;

	}

	for (uint32_t i = 0; i < divisionNumber; i++) {
		Vector2 first_ = CatmullRom[i];
		Vector2 second_ = CatmullRom[i + 1];

		length_ = (second_ - first_).Length();
		lengthNum += length_;
		if (i + drawCount_ * divisionNumber < lines_.size()) {
			lines_[i + drawCount_ * divisionNumber]->Draw(first_, second_, viewProjectionMatrix, Linecolor_);
		}

	}
	if (catMullLength_.size() < ControlPoints_.size() - 1) {
		catMullLength_.push_back(lengthNum);
	}
	else {
		catMullLength_[drawCount_] = lengthNum;
	}

	drawCount_++;


}

void TitleScene::MoveTexture(){
	if (isMove_) {
		t_ += moveSpeeds_[linePass_];
	}
	if (t_ >= 1.0f) {
		t_ = 0.0f;
		linePass_++;
	}

	if (linePass_ == 0) {
		runTexture_->transform.translate= Vector2::CatmullRom(ControlPoints_[0], ControlPoints_[0],
			ControlPoints_[1], ControlPoints_[2], t_);
	}
	else if (linePass_ == 1) {
		runTexture_->transform.translate = Vector2::CatmullRom(ControlPoints_[0], ControlPoints_[1],
			ControlPoints_[2], ControlPoints_[3], t_);
	}
	else if (linePass_ == 2) {
		runTexture_->transform.translate = Vector2::CatmullRom(ControlPoints_[1], ControlPoints_[2],
			ControlPoints_[3], ControlPoints_[3], t_);
	}
	else {
		isMove_ = false;
	}




}


void TitleScene::SetControlPoint(){
	//第一ポイントの位置によって+-を変更
	//画面の左側なら
	if (ControlPoints_[0].x < ControlPoints_[3].x) {
		ControlPoints_[1] = ControlPoints_[0] + Vector2(-secondPointPos_.x, secondPointPos_.y);
	}
	else {
		ControlPoints_[1] = ControlPoints_[0] + Vector2(secondPointPos_.x, secondPointPos_.y);
	}

	if (ControlPoints_[0].x < ControlPoints_[3].x) {
		ControlPoints_[2] = ((ControlPoints_[1] + ControlPoints_[3]) / 2.0f) + Vector2(-thirdPointPos_.x, thirdPointPos_.y);
	}
	else {
		ControlPoints_[2] = ((ControlPoints_[1] + ControlPoints_[3]) / 2.0f) + Vector2(thirdPointPos_.x, thirdPointPos_.y);
	}

	
}

void TitleScene::Debug(){
	ImGui::Begin("点の位置");
	for (size_t i = 0; i < ControlPoints_.size(); ++i) {
		ImGui::DragFloat2(("Points" + std::to_string(i)).c_str(), ControlPoints_[i].data(), 1.0f);
	}
	ImGui::End();

	ImGui::Begin("線の長さ");

	for (size_t i = 0; i < catMullLength_.size(); ++i) {
		ImGui::DragFloat(("lines" + std::to_string(i) + "&" + std::to_string(i + 1)).c_str(), &catMullLength_[i], 0.1f);
	}
	ImGui::End();


	ImGui::Begin("MakeCatmull-Rom");


	//if (ImGui::Button("線の追加")) {
	//	Vector3 newPoint = { 0.0f,0.0f,0.0f };		

	//	LastLinePass++;

	//	ControlPoints.push_back(newPoint);
	//	if ((ControlPoints.size() - 1) * 8 > lines_.size()){
	//		for (int i = 0; i < 8; i++) {
	//			lines_.push_back(std::make_unique<Line>());
	//		}
	//	}
	//}

	//ImGui::InputInt("何個追加するか", &addElementsNum_);

	//if (addElementsNum_ < 1) {
	//	addElementsNum_ = 1;
	//}

	//if (ImGui::Button("線の追加")) {
	//	for (int y = 0; y < addElementsNum_; y++) {
	//		Vector2 newPoint = { 0.0f,0.0f };

	//		LastLinePass_++;

	//		ControlPoints_.push_back(newPoint);
	//		if ((ControlPoints_.size() - 1) * divisionNumber_ > lines_.size()) {
	//			for (uint32_t i = 0; i < divisionNumber_; i++) {
	//				lines_.push_back(std::make_unique<Line>());
	//			}
	//		}
	//	}
	//}

	//ImGui::InputInt("何個削除するか", &subtractionElementsNum_);

	//if (subtractionElementsNum_ < 1) {
	//	subtractionElementsNum_ = 1;
	//}

	//ImGui::RadioButton("一番前から", &mode_, First);
	//ImGui::SameLine();
	//ImGui::RadioButton("一番後ろから", &mode_, Last);



	//if (ImGui::Button("線の削除")) {
	//	if (mode_ == First) {
	//		for (int i = 0; i < subtractionElementsNum_; i++) {
	//			if (ControlPoints_.size() <= 4) {
	//				break;
	//			}
	//			if (!ControlPoints_.empty()) {
	//				LastLinePass_--;

	//				ControlPoints_.erase(ControlPoints_.begin());
	//			}
	//		}
	//	}
	//	else if (mode_ == Last) {
	//		for (int i = 0; i < subtractionElementsNum_; i++) {
	//			if (ControlPoints_.size() <= 4) {
	//				break;
	//			}
	//			if (!ControlPoints_.empty()) {
	//				LastLinePass_--;

	//				ControlPoints_.pop_back();
	//			}
	//		}
	//	}

	//}

	///*if (ImGui::Button("最後の線を削除")){
	//	if (!ControlPoints.empty()) {
	//		LastLinePass--;

	//		ControlPoints.pop_back();

	//	}
	//}*/
	if (ImGui::Button("テクスチャを動かす")) {
		isMove_ = true;
		linePass_ = 0;
	}
	ImGui::DragFloat("第一区間のテクスチャの動く速度", &moveSpeeds_[0], 0.001f, 0.001f, 0.2f);
	ImGui::DragFloat("第二区間のテクスチャの動く速度", &moveSpeeds_[1], 0.001f, 0.001f, 0.2f);
	ImGui::DragFloat("第三区間のテクスチャの動く速度", &moveSpeeds_[2], 0.001f, 0.001f, 0.2f);
	ImGui::DragFloat2("第二点の位置関係", secondPointPos_.data(), 1.0f);
	ImGui::DragFloat2("第三点の位置関係", thirdPointPos_.data(), 1.0f);
	ImGui::Text("ControlPointsの数 = %d", ControlPoints_.size());
	ImGui::Text("今の最大サイズ = %d", lines_.size());
	ImGui::Text("線の合計本数 = %d", (ControlPoints_.size() - 1) * 8);

	ImGui::End();

}
