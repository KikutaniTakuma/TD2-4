#include "PlayerBlockPickerComp.h"
#include <Drawers/DrawerManager.h>
#include <SoLib/Math/Math.hpp>
#include "DwarfComp.h"
#include "DwarfAnimator.h"
#include "PlayerComp.h"

void PlayerBlockPickerComp::Init()
{
	pLocalBodyComp_ = object_.GetComponent<LocalBodyComp>();
	pBlockMap_ = GameManager::GetInstance()->GetMap();

	targetFlameTex_ = TextureManager::GetInstance()->LoadTexture("./Resources/Player/pressPositionFram.png");

	pModel_ = DrawerManager::GetInstance()->GetModel("Resources/Cube.obj");

	if (not pTexture_) [[unlikely]]
		{
			pTexture_ = DrawerManager::GetInstance()->GetTexture2D();
			texID_ = TextureManager::GetInstance()->GetWhiteTex();
	};

		
	noSpace_ = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/noSpace.mp3");
	pickUp_ = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/pickUp.mp3");
	putBlock_ = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/blockDropOff.mp3");
}

void PlayerBlockPickerComp::Update()
{
	affine_ = SoLib::Math::Affine(Vector3::kIdentity * 0.5f, Vector3::kZero, { pLocalBodyComp_->GetGlobalPos() + Vector2::kYIdentity, -1.f });

	Lamb::SafePtr playerComp = object_.GetComponent<PlayerComp>();
	Vector2 targetPos = pLocalBodyComp_->localPos_ + Vector2::kXIdentity * static_cast<float>(playerComp->GetFacing()) + Vector2::kIdentity * 0.5f;
	if (playerComp->InputDown()) {
		targetPos = pLocalBodyComp_->localPos_ + Vector2::kIdentity * 0.5f - Vector2::kYIdentity;
	}

	const POINTS pos = { .x = static_cast<int16_t>(targetPos.x), .y = static_cast<int16_t>(targetPos.y) };

	const auto &block = pBlockMap_->GetBlockType(pos);
	targetIsBlock_ = block != Block::BlockType::kNone;

	targetAffine_ = SoLib::Math::Affine(Vector3::kIdentity, Vector3::kZero, { BlockMap::GetGlobalPos(pos) , -5.f });

	for (auto &dwarf : dwarfList_) {
		//dwarf.second->transform_.

		dwarf.second->GetComponent<LocalBodyComp>()->localPos_ = pLocalBodyComp_->localPos_ + Vector2::kYIdentity * (pickingBlock_ ? 2.f : 1.f);
		dwarf.second->transform_.translate = dwarf.second->GetComponent<LocalBodyComp>()->GetGlobalPos();
		dwarf.second->transform_.CalcMatrix();
		dwarf.second->GetComponent<SpriteComp>()->Update();
		dwarf.second->GetComponent<DwarfAnimatorComp>()->Update();
		dwarf.second->GetComponent<SpriteAnimatorComp>()->Update();
	}
}

void PlayerBlockPickerComp::Draw([[maybe_unused]] const Camera &camera) const
{
	uint32_t color;
	if (IsPicking()) {
		color = targetIsBlock_ ? 0xFF0000FF : 0xFFFFFFFF;
	}
	else {
		color = targetIsBlock_ ? 0xFFFFFFFF : 0xFF0000FF;
	}

	pTexture_->Draw(targetAffine_, Mat4x4::MakeScalar({ 0.5f,1.f,1.f }), camera.GetViewOthographics(), targetFlameTex_, color, BlendType::kNormal);

	// もしブロックを持っていたら
	if (pickingBlock_) {
		pModel_->Draw(affine_, camera.GetViewProjection(), pickingBlock_.GetColor(), BlendType::kNormal, false);
		//pTexture_->Draw(affine_, pickingBlock_.GetDamageUv(), camera.GetViewOthographics(), pickingBlock_.GetTexture(), 0xFFFFFFFF, BlendType::kNone);

	}
	for (const auto &dwarf : dwarfList_) {
		dwarf.second->Draw(camera);
	}
}

void PlayerBlockPickerComp::PickUp(int32_t facing)
{
	// ブロックを持っていない場合
	if (not pickingBlock_ and dwarfList_.empty()) {
		Vector2 targetPos = pLocalBodyComp_->localPos_ + Vector2::kXIdentity * static_cast<float>(facing) + Vector2::kIdentity * 0.5f;
		if (facing == 0) {
			targetPos = pLocalBodyComp_->localPos_ + Vector2::kIdentity * 0.5f - Vector2::kYIdentity;
		}

		const POINTS pos = { .x = static_cast<int16_t>(targetPos.x), .y = static_cast<int16_t>(targetPos.y) };
		if (BlockMap::IsOutSide(pos)) { return; }
		auto &block = pBlockMap_->GetBlockMap()->at(pos.y).at(pos.x);

		if (block) {


			pickingBlock_ = block;

			pBlockMap_->BreakBlock(pos);

			dwarfList_ = GameManager::GetInstance()->PickUpObject(POINTS{ pos.x, static_cast<int16_t>(pos.y + 1) });

			pickUp_->Start(0.15f, false);
		}
		else {
			dwarfList_ = GameManager::GetInstance()->PickUpObject(POINTS{ pos.x, static_cast<int16_t>(pos.y) });
			//pickUp_->Start(0.1f, false);
		}

		facing_ = facing;
	}
}

void PlayerBlockPickerComp::Drop(int32_t facing)
{
	// ブロックを持っている場合
	if (pickingBlock_ or not dwarfList_.empty()) {
		Vector2 targetPos = pLocalBodyComp_->localPos_ + Vector2::kXIdentity * static_cast<float>(facing) + Vector2::kIdentity * 0.5f;
		if (facing == 0) {
			targetPos = pLocalBodyComp_->localPos_ + Vector2::kIdentity * 0.5f - Vector2::kYIdentity;
		}

		auto *gameManager = GameManager::GetInstance();

		if (BlockMap::IsOutSide(targetPos)) { return; }

		// ブロックを持っていなかったら
		if (not pickingBlock_) {

			POINTS pos = { .x = static_cast<int16_t>(targetPos.x), .y = static_cast<int16_t>(targetPos.y) };


			for (auto &dwarf : dwarfList_) {
				dwarf.second->GetComponent<LocalBodyComp>()->localPos_ = { static_cast<float>(pos.x),static_cast<float>(pos.y) };
				gameManager->AddDwarf(std::move(dwarf.second));
			}
			dwarfList_.clear();
			return;
		}

		auto targetBlock = pBlockMap_->GetBlockType(targetPos);

		if (targetBlock == Block::BlockType::kNone) {

			POINTS pos = { .x = static_cast<int16_t>(targetPos.x), .y = static_cast<int16_t>(targetPos.y) };
			//// 下側が空なら落下ブロックを設置する
			//if (pBlockMap_->GetBlockType(targetPos - Vector2::kYIdentity) == Block::BlockType::kNone) {
			//	gameManager->AddFallingBlock(Vector2{ static_cast<float>(pos.x), static_cast<float>(pos.y) }, Vector2::kIdentity, pickingBlock_.GetBlockType(), Vector2::kYIdentity * -5, Vector2::kZero, false);
			//}
			//else {
			(*pBlockMap_->GetBlockMap())[pos.y][pos.x] = pickingBlock_;
			//}

			for (auto &dwarf : dwarfList_) {
				dwarf.second->GetComponent<LocalBodyComp>()->localPos_ = { static_cast<float>(pos.x),static_cast<float>(pos.y) + 1 };
				gameManager->AddDwarf(std::move(dwarf.second));
			}
			dwarfList_.clear();


			pickingBlock_ = Block::BlockType::kNone;
			putBlock_->Start(0.2f, false);
		}
		else {
			noSpace_->Start(0.2f, false);
		}
	}
	else {
		noSpace_->Start(0.2f, false);
	}
}
