#include "SpriteAnimatorComp.h"

void SpriteAnimatorComp::Init()
{
	pSpriteComp_ = object_.AddComponent<SpriteComp>();
	textureAnimator_ = std::make_unique<Tex2DAniamtor>();
}

void SpriteAnimatorComp::Update()
{
	textureAnimator_->Update();
	pSpriteComp_->uvMatrix_ = textureAnimator_->GetUvMat4x4();
}
