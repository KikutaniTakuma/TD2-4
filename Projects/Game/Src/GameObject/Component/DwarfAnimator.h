#pragma once
#include <array>
#include "../GameObject.h"

#include "Utils/SafePtr/SafePtr.h"

#include "DwarfComp.h"			// 小人の動作コンポーネント
#include "SpriteComp.h"			// 小人自体のスプライトのコンポーネント
#include "SpriteAnimatorComp.h"	// スプライトに紐づいたアニメーションのコンポーネント

#include "Game/FlaskParticle/FlaskParticle.h"

class DwarfAnimatorComp : public IComponent
{
	enum DamageTex {
		kNormal,
		kDamage,
	};

public:
	using IComponent::IComponent;
	~DwarfAnimatorComp() = default;

	static void StaticLoad();

	void Init() override;

	void Update() override;
	void Draw(const Camera &camera) const override;

private:

	Lamb::SafePtr<DwarfComp> pDwarfComp_ = nullptr;
	Lamb::SafePtr<SpriteComp> pSpriteComp_ = nullptr;
	Lamb::SafePtr<SpriteAnimatorComp> pAnimComp_ = nullptr;

	Lamb::SafePtr<Tex2DAniamtor> spriteAnimator_ = nullptr;

	inline static std::array<std::array<uint32_t, 4u>, 2u> textureID_;

	Lamb::Flg isDrakDwarf_;

	std::unique_ptr<FlaskParticle> absorptionParticle_;
};