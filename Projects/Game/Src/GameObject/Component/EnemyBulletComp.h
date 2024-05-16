#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"
#include "LocalMapHitComp.h"
#include "LocalRigidbody.h"
#include "LocalBodyComp.h"
#include "SpriteComp.h"

class EnemyBulletComp : public IComponent {
public:
	using IComponent::IComponent;

	void Init() override;

	void Update() override;

private:

	Lamb::SafePtr<LocalMapHitComp> pHitMapComp_ = nullptr;
	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;
	Lamb::SafePtr<LocalRigidbody> pLocalRigidbody_ = nullptr;
	Lamb::SafePtr<SpriteComp> pSpriteComp_ = nullptr;
};
