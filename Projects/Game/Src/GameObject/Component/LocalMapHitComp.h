#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Game/Map/BlockMap.h"
#include "LocalRigidbody.h"

class LocalMapHitComp : public IComponent {
public:
	using IComponent::IComponent;

	void Init() override;

	void Update() override;

	Vector2 hitNormal_;

	bool isHitFallBlock_ = true;

private:

	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;
	Lamb::SafePtr<LocalRigidbody> pRigidbody_ = nullptr;
	Lamb::SafePtr<BlockMap> pBlockMap_ = nullptr;

};