#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"
#include "LocalMapHitComp.h"
#include "LocalRigidbody.h"
#include "LocalBodyComp.h"
#include "SpriteComp.h"

#include "Drawers/Particle/Particle.h"

class EnemyBulletComp : public IComponent {
public:
	using IComponent::IComponent;

	static void StaticLoad();

	void Init() override;

	void Update() override;

	void OnCollision(GameObject *const other) override;

	void Draw(const Camera &camera) const override;
private:

	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;
	Lamb::SafePtr<LocalRigidbody> pLocalRigidbody_ = nullptr;
	Lamb::SafePtr<SpriteComp> pSpriteComp_ = nullptr;

	std::unique_ptr<Particle> shotParticle_;
};
