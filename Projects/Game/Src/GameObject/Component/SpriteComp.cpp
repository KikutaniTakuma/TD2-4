#include "SpriteComp.h"
#include "Drawers/DrawerManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

void SpriteComp::Init()
{
	// 親のトランスフォームを指定
	offsetTransform_.parent_ = &transform_;
	if (not pTexture_) [[unlikely]]
		{
			pTexture_ = DrawerManager::GetInstance()->GetTexture2D();
		}
}

void SpriteComp::Update()
{
	transform_.CalcMatrix();
	offsetTransform_.CalcMatrix();
}

void SpriteComp::Draw(const Camera &camera) const
{
	pTexture_->Draw(offsetTransform_.matWorld_, uvMatrix_, camera.GetViewOthographics(), textureID_, color_, blendType_);
}

void SpriteComp::SetTexture(uint32_t textureID)
{
	textureID_ = textureID;
}

void SpriteComp::SetTexture(const std::string &filePath)
{
	textureID_ = TextureManager::GetInstance()->LoadTexture(filePath);
}

void SpriteComp::CalcTexUv()
{
	uvMatrix_ = uvTransform_.MakeAffine();

}

