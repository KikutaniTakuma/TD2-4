#include "BlockItem.h"
#include <Drawers/DrawerManager.h>

BlockItem::BlockItem(const Vector2& startPos, const Vector2& endPos, const Block::BlockType& type) {
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();
	texState_ = std::make_unique<Tex2DState>();
	texState_->color = 0xffffffff;
	texState_->transform.scale = Vector2::kIdentity;
	texState_->transform.translate = startPos;
	controlPoints_[0] = startPos;
	controlPoints_[3] = endPos;

	SetControlPosition();

	switch (type){
	case Block::BlockType::kRed:
		texState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Item/lizardTail.png");
		texState_->textureFullPath = "./Resources/Item/lizardTail.png";
		texState_->textureName = "lizardTail";
		break;

	case Block::BlockType::kBlue:
		texState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Item/water.png");
		texState_->textureFullPath = "./Resources/Item/water.png";
		texState_->textureName = "water";
		break;

	case Block::BlockType::kYellow :
		texState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Item/mineral.png");
		texState_->textureFullPath = "./Resources/Item/mineral.png";
		texState_->textureName = "mineral";
		break;

	case Block::BlockType::kGreen:
		texState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Item/herbs.png");
		texState_->textureFullPath = "./Resources/Item/herbs.png";
		texState_->textureName = "herbs";
		break;
	default:
		std::string message = "Item could not be generated successfully.";
		throw Lamb::Error::Code<BlockItem>(message, ErrorPlace);
	}

	moveSpeeds_ = { 0.3f,0.3f,0.3f };

	isMove_ = true;
	
}

void BlockItem::Update(const float deltaTime){

	MoveTexture(deltaTime);

	texState_->transform.CalcMatrix();
}

void BlockItem::Draw(const Mat4x4& cameraMat){
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

void BlockItem::SetControlPosition(){
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


