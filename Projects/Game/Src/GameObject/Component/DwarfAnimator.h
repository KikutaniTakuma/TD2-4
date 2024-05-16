#pragma once
#include "../GameObject.h"

#include "Utils/SafePtr/SafePtr.h"

#include "DwarfComp.h"			// 小人の動作コンポーネント
#include "SpriteComp.h"			// 小人自体のスプライトのコンポーネント
#include "SpriteAnimatorComp.h"	// スプライトに紐づいたアニメーションのコンポーネント

class DwarfAnimatorComp : public IComponent
{
public:
	using IComponent::IComponent;
	~DwarfAnimatorComp() = default;

	void Init() override;

	void Update() override;

private:

	Lamb::SafePtr<DwarfComp> pDwarfComp_ = nullptr;
	Lamb::SafePtr<SpriteComp> pSpriteComp_ = nullptr;
	Lamb::SafePtr<SpriteAnimatorComp> pAnimComp_ = nullptr;

	Lamb::SafePtr<Tex2DAniamtor> spriteAnimator_ = nullptr;

};