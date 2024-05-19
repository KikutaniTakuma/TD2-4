#include "PlayerBlockPickerComp.h"
#include <Drawers/DrawerManager.h>
#include <SoLib/Math/Math.hpp>
#include "DwarfComp.h"
#include "DwarfAnimator.h"

void PlayerBlockPickerComp::Init()
{
	pLocalBodyComp_ = object_.GetComponent<LocalBodyComp>();
	pBlockMap_ = GameManager::GetInstance()->GetMap();
	if (not pTexture_) [[unlikely]]
		{
			pTexture_ = DrawerManager::GetInstance()->GetTexture2D();
			texID_ = TextureManager::GetInstance()->GetWhiteTex();
		}
}

void PlayerBlockPickerComp::Update()
{
	affine_ = SoLib::Math::Affine(Vector3::kIdentity, Vector3::kZero, { pLocalBodyComp_->GetGlobalPos() + Vector2::kYIdentity, -5.f });

	for (auto &dwarf : dwarfList_) {
		//dwarf.second->transform_.

		dwarf.second->GetComponent<LocalBodyComp>()->localPos_ = pLocalBodyComp_->localPos_ + Vector2::kYIdentity * 2.f;
		dwarf.second->transform_.translate = dwarf.second->GetComponent<LocalBodyComp>()->GetGlobalPos();
		dwarf.second->transform_.CalcMatrix();
		dwarf.second->GetComponent<SpriteComp>()->Update();
		dwarf.second->GetComponent<DwarfAnimatorComp>()->Update();
		dwarf.second->GetComponent<SpriteAnimatorComp>()->Update();
	}
}

void PlayerBlockPickerComp::Draw([[maybe_unused]] const Camera &camera) const
{
	// もしブロックを持っていたら
	if (pickingBlock_) {
		pTexture_->Draw(affine_, Mat4x4::kIdentity, camera.GetViewOthographics(), pickingBlock_.GetTexture(), 0xFFFFFFFF, BlendType::kNone);

		for (const auto &dwarf : dwarfList_) {
			dwarf.second->Draw(camera);
		}
	}
}

void PlayerBlockPickerComp::PickUp(int32_t facing)
{
	// ブロックを持っていない場合
	if (not pickingBlock_) {
		Vector2 targetPos = pLocalBodyComp_->localPos_ + Vector2::kXIdentity * static_cast<float>(facing) + Vector2::kIdentity * 0.5f;

		auto block = pBlockMap_->GetBlockType(targetPos);

		if (block != Block::BlockType::kNone) {

			POINTS pos = { .x = static_cast<int16_t>(targetPos.x), .y = static_cast<int16_t>(targetPos.y) };
			pBlockMap_->BreakBlock(pos);

			dwarfList_ = GameManager::GetInstance()->PickUpBlockSideObject(pos);

			pickingBlock_ = block;

		}
	}
}

void PlayerBlockPickerComp::Drop(int32_t facing)
{
	// ブロックを持っている場合
	if (pickingBlock_) {
		Vector2 targetPos = pLocalBodyComp_->localPos_ + Vector2::kXIdentity * static_cast<float>(facing) + Vector2::kIdentity * 0.5f;

		auto block = pBlockMap_->GetBlockType(targetPos);

		if (block == Block::BlockType::kNone) {

			POINTS pos = { .x = static_cast<int16_t>(targetPos.x), .y = static_cast<int16_t>(targetPos.y) };
			(*pBlockMap_->GetBlockMap())[pos.y][pos.x] = pickingBlock_;

			auto *gameManager = GameManager::GetInstance();

			for (auto &dwarf : dwarfList_) {
				dwarf.second->GetComponent<LocalBodyComp>()->localPos_ = { static_cast<float>(pos.x),static_cast<float>(pos.y) + 1 };
				gameManager->AddDwarf(std::move(dwarf.second));
			}
			dwarfList_.clear();


			pickingBlock_ = Block::BlockType::kNone;

		}
	}
}
