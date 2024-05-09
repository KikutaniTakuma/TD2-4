#include "FallingBlockSpawnerComp.h"
#include <Drawers/DrawerManager.h>
#include <SoLib/Math/Math.hpp>


void FallingBlockSpawnerComp::Init()
{
	pLocalPosComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalPosComp_->size_ = Vector2::kIdentity;
	pLocalPosComp_->drawScale_ = 1.f;

	fallBlockModel_ = DrawerManager::GetInstance()->GetModel("Resources/Cube.obj");

	fallBlockType_.SetBlockType(Block::BlockType::kRed);
}

void FallingBlockSpawnerComp::Start()
{
	pLocalPosComp_->localPos_ = Vector2{ 0, static_cast<float>(BlockMap::kMapY) };
}

void FallingBlockSpawnerComp::Update()
{

	pLocalPosComp_->TransfarData();

}

void FallingBlockSpawnerComp::Draw(const Camera &camera) const
{

	if (startPos_ != -1) {
		float xDiff = startPos_ - pLocalPosComp_->localPos_.x;
		xDiff = std::min(std::abs(xDiff), static_cast<float>(GetMaxBlockWidth() - 1)) * SoLib::Math::Sign(xDiff);

		const Vector2 spawnPos{ static_cast<float>(startPos_), pLocalPosComp_->localPos_.y };

		Vector3 pos = { BlockMap::GetGlobalPos(spawnPos) - Vector2::kYIdentity - Vector2::kXIdentity * (xDiff * 0.5f), -1.f };
		Matrix affine = SoLib::Math::Affine(Vector3{ std::abs(xDiff) + 1 ,1,1 } / 2, Vector3::kZero, pos);

		fallBlockModel_->Draw(affine, camera.GetViewOthographics(), fallBlockType_.GetColor(), BlendType::kNone);
	}
}

void FallingBlockSpawnerComp::MoveInput(int32_t xMove)
{
	pLocalPosComp_->localPos_.x += xMove;
	pLocalPosComp_->localPos_.x = std::clamp(pLocalPosComp_->localPos_.x, 0.f, static_cast<float>(BlockMap::kMapX - 1));
}

void FallingBlockSpawnerComp::SetStartPos()
{
	if (GetMaxBlockWidth() > 0) {
		startPos_ = static_cast<int32_t>(pLocalPosComp_->localPos_.x);
	}
}

int32_t FallingBlockSpawnerComp::GetMaxBlockWidth() const
{
	return std::min(pBlockGauge_->GetBlockCount(), kMaxWidth_);
}

void FallingBlockSpawnerComp::SetGauge(BlockGauge *pBlockGauge)
{
	pBlockGauge_ = pBlockGauge;
}

std::pair<int32_t, int32_t> FallingBlockSpawnerComp::GetFutureBlockPos() const
{
	std::pair<int32_t, int32_t> result{ -1, -1 };
	// もしブロックを設置してなかったら飛ばす
	if (startPos_ != -1) {
		int32_t xDiff = startPos_ - static_cast<int32_t>(pLocalPosComp_->localPos_.x);
		xDiff = std::min(std::abs(xDiff), GetMaxBlockWidth() - 1) * SoLib::Math::Sign(xDiff);

		result.first = startPos_;
		result.second = startPos_ - xDiff;
	}
	return result;
}

void FallingBlockSpawnerComp::SpawnFallingBlock()
{
	if (startPos_ != -1) {
		int32_t xDiff = startPos_ - static_cast<int32_t>(pLocalPosComp_->localPos_.x);
		xDiff = std::min(std::abs(xDiff), GetMaxBlockWidth() - 1) * SoLib::Math::Sign(xDiff);

		const Vector2 spawnPos{ static_cast<float>(startPos_), pLocalPosComp_->localPos_.y };


		// 落下ブロックの実体の追加
		pLocalPosComp_->pGameManager_->AddFallingBlock(spawnPos - Vector2::kYIdentity - Vector2::kXIdentity * (xDiff * 0.5f), Vector2::kIdentity + Vector2::kXIdentity * static_cast<float>(std::abs(xDiff)), fallBlockType_.GetBlockType(), Vector2::kYIdentity * -10, Vector2::kZero);

		// 使用量を減らす
		pBlockGauge_->UseBlockEnergy(std::abs(xDiff) + 1);

		startPos_ = -1;
	}
}
