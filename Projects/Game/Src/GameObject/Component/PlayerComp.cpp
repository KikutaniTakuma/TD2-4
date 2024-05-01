#include "PlayerComp.h"
#include <Drawers/DrawerManager.h>
#include <SoLib/Math/Math.hpp>


void PlayerComp::Init()
{
	pLocalPosComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalPosComp_->size_ = Vector2::kIdentity;

	fallBlockModel_ = DrawerManager::GetInstance()->GetModel("Resources/Cube.obj");
}

void PlayerComp::Start()
{
	pLocalPosComp_->localPos_ = Vector2{ 0,static_cast<float>(Map::kMapY) };
}

void PlayerComp::Update()
{

	pLocalPosComp_->TransfarData();

}

void PlayerComp::Draw(const Camera &camera) const
{

	if (startPos_ != -1) {
		float xDiff = startPos_ - pLocalPosComp_->localPos_.x;

		Vector3 pos = Map::GetGrobalPos(pLocalPosComp_->localPos_) - Vector2::kYIdentity + Vector2::kXIdentity * (xDiff * 0.5f);
		Matrix affine = SoLib::Math::Affine(Vector3{ std::abs(xDiff) + 1 ,1,1 } / 2, Vector3::kZero, pos);

		fallBlockModel_->Draw(affine, camera.GetViewProjection(), 0xFFFFFFFF, BlendType::kNone);
	}
}

void PlayerComp::MoveInput(int32_t xMove)
{
	pLocalPosComp_->localPos_.x += xMove;
	pLocalPosComp_->localPos_.x = std::clamp(pLocalPosComp_->localPos_.x, 0.f, static_cast<float>(Map::kMapX - 1));
}

void PlayerComp::SetStartPos()
{
	startPos_ = static_cast<int32_t>(pLocalPosComp_->localPos_.x);
}

void PlayerComp::SpawnFallingBlock()
{

	int32_t xDiff = startPos_ - static_cast<int32_t>(pLocalPosComp_->localPos_.x);

	// 落下ブロックの実体の追加
	pLocalPosComp_->pGameManager_->AddFallingBlock(pLocalPosComp_->localPos_ - Vector2::kYIdentity + Vector2::kXIdentity * (xDiff * 0.5f), Vector2::kIdentity + Vector2::kXIdentity * static_cast<float>(std::abs(xDiff)), false, Vector2::kYIdentity * -20, Vector2::kZero);

	startPos_ = -1;
}
