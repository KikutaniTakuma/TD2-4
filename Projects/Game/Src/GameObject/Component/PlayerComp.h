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

	void Init() override;

	void Update() override;

	void OnCollision(GameObject *const other) override;

	inline static SoLib::VItem<"無敵時間", float> vMaxInvincibleTime_ = { 1.f };
	inline static SoLib::VItem<"最大体力", float> vMaxHealth_ = { 10.f };

private:

	float invincibleTime_ = 0.f;

	void FireBullet();

	void Input();
	inline static const Vector2 kGrovity_{ 0,-40.f };

	int32_t facing_ = 1;

	Lamb::SafePtr<LocalMapHitComp> pHitMapComp_ = nullptr;
	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;
	Lamb::SafePtr<LocalRigidbody> pLocalRigidbody_ = nullptr;
	Lamb::SafePtr<PleyerBlockPickerComp> pPicker_ = nullptr;

	Lamb::SafePtr<HealthComp> pHealthComp_ = nullptr;

};