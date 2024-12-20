#include "BlockItem.h"
#include <Drawers/DrawerManager.h>

BlockItem::BlockItem(const Vector2 &startPos, const Vector2 &endPos, const Block::BlockType &type, float startTime) {
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();
	texState_ = std::make_unique<Tex2DState>();
	texState_->color = 0xffffffff;
	texState_->transform.scale = Vector2::kIdentity;
	texState_->transform.translate = { startPos.x,startPos.y,-1.0f };
	controlPoints_[0] = startPos;
	controlPoints_[3] = endPos;

	SetControlPosition();

	texState_->textureID = Block::GetItemTexture(type);

	itemType_ = type;

	moveSpeeds_ = { 2.0f,2.0f,2.0f };

	startTimer_.Start(startTime);

	//isMove_ = true;

}

void BlockItem::Update(const float deltaTime) {
	startTimer_.Update(deltaTime);
	if (startTimer_.IsFinish()) {
		isMove_ = true;
	}

	MoveTexture(deltaTime);

	texState_->transform.CalcMatrix();
}

void BlockItem::Draw(const Mat4x4 &cameraMat) {
	tex2D_->Draw(texState_->transform.matWorld_, Mat4x4::kIdentity, cameraMat,
		texState_->textureID, texState_->color, BlendType::kNormal);
}

void BlockItem::MoveTexture(const float deltaTime) {
	if (isMove_) {
		t_ += moveSpeeds_[linePass_] * deltaTime;
	}
	if (t_ >= 1.0f) {
		t_ = 0.0f;
		linePass_++;
	}

	if (linePass_ == 0) {
		texState_->transform.translate = Vector2::CatmullRom(controlPoints_[0], controlPoints_[0],
			controlPoints_[1], controlPoints_[2], t_);
	}
	else if (linePass_ == 1) {
		texState_->transform.translate = Vector2::CatmullRom(controlPoints_[0], controlPoints_[1],
			controlPoints_[2], controlPoints_[3], t_);
	}
	else if (linePass_ == 2) {
		texState_->transform.translate = Vector2::CatmullRom(controlPoints_[1], controlPoints_[2],
			controlPoints_[3], controlPoints_[3], t_);
	}
	else {
		isMove_ = false;
		isEndMove_ = true;
	}
}

void BlockItem::SetControlPosition() {
	//第一ポイントの位置によって+-を変更
	//画面の左側なら
	if (controlPoints_[0].x < controlPoints_[3].x) {
		controlPoints_[1] = controlPoints_[0] + Vector2(-secondPointPos_.x, secondPointPos_.y);
	}
	else {
		controlPoints_[1] = controlPoints_[0] + Vector2(secondPointPos_.x, secondPointPos_.y);
	}

	if (controlPoints_[0].x < controlPoints_[3].x) {
		controlPoints_[2] = ((controlPoints_[1] + controlPoints_[3]) / 2.0f) + Vector2(-thirdPointPos_.x, thirdPointPos_.y);
	}
	else {
		controlPoints_[2] = ((controlPoints_[1] + controlPoints_[3]) / 2.0f) + Vector2(thirdPointPos_.x, thirdPointPos_.y);
	}
}


