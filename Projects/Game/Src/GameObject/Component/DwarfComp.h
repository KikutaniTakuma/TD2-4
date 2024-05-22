#pragma once
#include "../GameObject.h"
#include "AudioManager/AudioManager.h"
#include "LocalBodyComp.h"
#include "PickUpComp.h"
#include "SoLib/Math/Math.hpp"
#include "Utils/SafePtr/SafePtr.h"
#include "LocalRigidbody.h"
#include "LocalMapHitComp.h"

class DwarfComp : public IComponent
{
public:
	// 移動速度
	inline static SoLib::VItem<"移動速度", float> vMovementSpeed_ = 1.5f;
	inline static SoLib::VItem<"壁を登るスピード", float> vClimbSpeed_ = 2.f;

	inline static SoLib::VItem<"弾の発射間隔", float> vBulletFireSpan_ = 5.f;
	inline static SoLib::VItem<"弾の速度", float> vBulletSpeed_ = 5.f;

	inline static constexpr SoLib::VItemList vDwarfItems_{ &vMovementSpeed_, &vBulletSpeed_, &vClimbSpeed_, &vBulletFireSpan_ };

public:
	using IComponent::IComponent;
	~DwarfComp() = default;

	void Init() override;

	void Start() override;

	void Update() override;

public:
	int32_t GetFacing() const { return facing_; }

	bool IsClimbing() const { return isClimbing_; }

	std::vector<void(DwarfComp:: *)(void)> updateFunc_;

	void ChangeMovementTarget();

	void FireBullet();

	bool GetIsDarkDwarf()const;

	const auto &AttackTimer() { return timer_; }

private:
	void ClimbUp();

	/// @brief 空中にいる場合は落下する
	/// @return 落下していたらtrue
	bool FallDown();

	void CarryBlockForHouse();

	int32_t GetLocatePosEnemyHouse() const;

	void LocateOnHouse();

	void FreeTargetMove();

	/// <summary>
	/// 移動を行う
	/// </summary>
	/// <param name="target">移動先</param>
	/// <returns>ゴールに到達したか</returns>
	bool MoveBlockPos(Vector2 target)
	{
		// ターゲットが指定されていない場合終わり
		if (target == -Vector2::kIdentity) {
			return false;
		}

		float xMove = target.x - pLocalBodyComp_->localPos_.x;
		pLocalBodyComp_->localPos_.x += std::clamp(xMove, -1.f, 1.f) * vMovementSpeed_ * GetDeltaTime();

		// 移動距離が有効なら
		if (xMove != 0) {
			// 方向を与える
			facing_ = static_cast<int32_t>(SoLib::Math::Sign(xMove));
		}

		return (target - pLocalBodyComp_->localPos_ + Vector2::kYIdentity).Length() < 0.1f;
	}

private:

	SoLib::Time::DeltaTimer timer_;

	int32_t facing_ = 1;

	int32_t movementFacing_;

	Vector2 targetPos_ = -Vector2::kIdentity;

	bool isClimbing_ = false;

	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;
	Lamb::SafePtr<PickUpComp> pPickUpComp_ = nullptr;
	Lamb::SafePtr<LocalRigidbody> pRigidbody_ = nullptr;
	Lamb::SafePtr<LocalMapHitComp> pMapHitComp_ = nullptr;

	Audio *killSE_ = nullptr;
};