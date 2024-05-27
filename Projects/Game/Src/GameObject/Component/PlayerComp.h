#pragma once
#include "../GameObject.h"
#include "LocalBodyComp.h"
#include "LocalRigidbody.h"
#include "LocalMapHitComp.h"
#include "PlayerBlockPickerComp.h"
#include "HealthComp.h"

class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() = default;

	static void StaticLoad();

	void Init() override;

	void Update() override;
	void Draw(const Camera &camera) const override;

	void OnCollision(GameObject *const other) override;

	inline static SoLib::VItem<"無敵時間", float> vMaxInvincibleTime_ = { 1.f };
	inline static SoLib::VItem<"最大体力", float> vMaxHealth_ = { 10.f };

	inline static SoLib::VItem<"移動加速度", float> vMoveSpeed_{ 10.f };
	inline static SoLib::VItem<"弾の速度", float> vBulletSpeed_{ 5.f };
	inline static SoLib::VItem<"停止時の地面との摩擦", float> vFriction_{ 8.f };
	inline static SoLib::VItem<"弾のクールタイム", float> vFireCoolTime_{ 0.25f };
	inline static SoLib::VItem<"被ダメージ時に落ちるアイテムの数", int32_t> vDamageDropCount_{ 10 };

	inline static constexpr SoLib::VItemList vPlayerItems_{ &PlayerComp::vMaxHealth_, &PlayerComp::vMaxInvincibleTime_, &PlayerComp::vMoveSpeed_, &PlayerComp::vBulletSpeed_, &PlayerComp::vFriction_, &vFireCoolTime_, &vDamageDropCount_ };

	int32_t InflictDamage(int32_t damage, const Vector2 acceleration = {});

	int32_t GetFacing() const {
		return facing_;
	}

	bool GetIsAttack() const {
		return isAttack_;
	}

	bool GetIsDamage() const;

	const Vector2 GetInputVec() const { return inputVec_; }

	const float GetInvincibleTime() const { return invincibleTime_; }

private:

	float invincibleTime_ = 0.f;

	float fireCoolTime_ = 0;

	void FireBullet();

	void Input();
	inline static const Vector2 kGrovity_{ 0,-40.f };

	Vector2 inputVec_;

	int32_t facing_ = 1;

	bool isAttack_;

	int32_t preHealth_;

	Lamb::SafePtr<LocalMapHitComp> pHitMapComp_ = nullptr;
	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;
	Lamb::SafePtr<LocalRigidbody> pLocalRigidbody_ = nullptr;
	Lamb::SafePtr<PlayerBlockPickerComp> pPicker_ = nullptr;

	Lamb::SafePtr<HealthComp> pHealthComp_ = nullptr;

};