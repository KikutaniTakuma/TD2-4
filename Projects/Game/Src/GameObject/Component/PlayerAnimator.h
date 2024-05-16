#pragma once

#pragma once
#include "../GameObject.h"

#include "Utils/SafePtr/SafePtr.h"

#include "PlayerComp.h"			// 小人の動作コンポーネント
#include "PickUpComp.h"			// 小人の持ち上げたブロックに関するコンポーネント
#include "SpriteComp.h"			// 小人自体のスプライトのコンポーネント
#include "SpriteAnimatorComp.h"	// スプライトに紐づいたアニメーションのコンポーネント
#include "PlayerBlockPickerComp.h"

#include <memory>
#include "Drawers/Particle/Particle.h"

class PlayerAnimatorComp : public IComponent
{
public:
	using IComponent::IComponent;
	~PlayerAnimatorComp() = default;

	void Init() override;

	void Update() override;

	void Draw(const Camera &camera) const override;

private:

	Lamb::SafePtr<PlayerComp> pPlayerComp_ = nullptr;
	Lamb::SafePtr<PlayerBlockPickerComp> pPlayerPickerComp_ = nullptr;
	Lamb::SafePtr<LocalRigidbody> pPlayerRigidComp_ = nullptr;
	Lamb::SafePtr<LocalMapHitComp> pMapHitComp_ = nullptr;
	Lamb::SafePtr<SpriteComp> pSpriteComp_ = nullptr;
	Lamb::SafePtr<SpriteAnimatorComp> pAnimComp_ = nullptr;

	Lamb::SafePtr<Tex2DAniamtor> spriteAnimator_ = nullptr;

	bool isAttackAnimation_;

	std::unique_ptr<Particle> haveParticle_;
	std::unique_ptr<Particle> shotParticle_;
	std::unique_ptr<Particle> smokeParticle_;
	std::unique_ptr<Particle> damageParticle_;

	Lamb::Flg isPicking_;

	Lamb::Flg isLanding_;
	Lamb::Flg isDamage_;

	Lamb::Flg isShoting_;
};