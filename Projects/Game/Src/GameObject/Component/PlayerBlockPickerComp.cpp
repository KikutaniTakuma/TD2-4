#include "PlayerBlockPickerComp.h"
#include <Drawers/DrawerManager.h>
#include <SoLib/Math/Math.hpp>

void PleyerBlockPickerComp::Init()
{
	pLocalBodyComp_ = object_.GetComponent<LocalBodyComp>();
	pBlockMap_ = GameManager::GetInstance()->GetMap();
	if (not pTexture_) [[unlikely]]
		{
			pTexture_ = DrawerManager::GetInstance()->GetTexture2D();
			texID_ = TextureManager::GetInstance()->GetWhiteTex();
		}
}

void PleyerBlockPickerComp::Update()
{
}

void PleyerBlockPickerComp::Draw([[maybe_unused]] const Camera &camera) const
{
	// もしブロックを持っていたら
	if (pickingBlock_) {
		Mat4x4 affine = SoLib::Math::Affine(Vector3::kIdentity, Vector3::kZero, { pLocalBodyComp_->GetGlobalPos() + Vector2::kYIdentity, -5.f });
		pTexture_->Draw(affine, Mat4x4::kIdentity, camera.GetViewOthographics(), texID_, pickingBlock_.GetColor(), BlendType::kNone);
	}
}

void PleyerBlockPickerComp::PickUp(int32_t facing)
{
	// ブロックを持っていない場合
	if (not pickingBlock_) {
		Vector2 targetPos = pLocalBodyComp_->localPos_ + Vector2::kXIdentity * static_cast<float>(facing);

		auto block = pBlockMap_->GetBlockType(targetPos);

		if (block != Block::BlockType::kNone) {

			POINTS pos = { .x = static_cast<int16_t>(targetPos.x), .y = static_cast<int16_t>(targetPos.y) };
			pBlockMap_->BreakBlock(pos);

			pickingBlock_ = block;

		}
	}
}
