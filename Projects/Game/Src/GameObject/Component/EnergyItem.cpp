#include "EnergyItem.h"

void EnergyItem::Init() {
	itemTexture_ = std::make_unique<Texture2D>();
	itemTexture_->ThreadLoadTexture("Resources/circle.png");
	itemTexture_->scale = { 100,100 };
}

void EnergyItem::Update() {
	itemTexture_->pos = transform_.translate;
	itemTexture_->Update();
}

void EnergyItem::Draw([[maybe_unused]] const Camera &camera) const {
	itemTexture_->Draw(camera.GetViewOthographics());
}
