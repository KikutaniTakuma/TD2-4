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
private:
	enum class State {
		kWiat,
		kMove,
		kAttack,
		kHave,
		kHaveUnder,
		kHaveMove
	};

public:
	using IComponent::IComponent;
	~PlayerAnimatorComp() = default;

	static void StaticLoad();

	void Init() override;

	void Update() override;

	void Draw(const Camera &camera) const override;

private:
	void SetState();

	void SetWait();
	void SetMove();
	void SetAttack();
	void SetHave();
	void SetHaveUnder();
	void SetHaveMove();

	void AttackAnimationUpdate();
	void HaveAnimationUpdate();
	void HaveUnderAnimationUpdate();


private:
	State currentState_ = State::kWiat;
	State preState_ = State::kWiat;

	Lamb::SafePtr<PlayerComp> pPlayerComp_ = nullptr;
	Lamb::SafePtr<PlayerBlockPickerComp> pPlayerPickerComp_ = nullptr;
	Lamb::SafePtr<LocalRigidbody> pPlayerRigidComp_ = nullptr;
	Lamb::SafePtr<LocalMapHitComp> pMapHitComp_ = nullptr;
	Lamb::SafePtr<SpriteComp> pSpriteComp_ = nullptr;
	Lamb::SafePtr<SpriteAnimatorComp> pAnimComp_ = nullptr;

	Lamb::SafePtr<Tex2DAniamtor> spriteAnimator_ = nullptr;

	std::unique_ptr<Particle> haveParticle_;
	std::unique_ptr<Particle> shotParticle_;
	std::unique_ptr<Particle> smokeParticle_;

	Lamb::Flg isHaveBlock_;
	Lamb::Flg isUnderHave_;
	Lamb::Flg isMove_;
	Lamb::Flg isAttack_;

	Lamb::Flg isLanding_;
	Lamb::Flg isDamage_;

	Lamb::Flg isShoting_;


	Lamb::Flg isUnderAnimation_;
	Lamb::Flg isUnderAnimationEnd_;

	Lamb::Flg isNormalPickAnimation_;
};