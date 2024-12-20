#pragma once
#include "../GameObject.h"

#include "Utils/SafePtr/SafePtr.h"

#include "DwarfComp.h"			// 小人の動作コンポーネント
#include "PickUpComp.h"			// 小人の持ち上げたブロックに関するコンポーネント
#include "SpriteComp.h"			// 小人自体のスプライトのコンポーネント

class DwarfShakeComp : public IComponent
{
public:
	using IComponent::IComponent;
	~DwarfShakeComp() = default;

	void Init() override;

	void Update() override;

private:

	Lamb::SafePtr<DwarfComp> pDwarfComp_ = nullptr;
	Lamb::SafePtr<PickUpComp> pPickUpComp_ = nullptr;
	Lamb::SafePtr<SpriteComp> pSpriteComp_ = nullptr;

	Vector2 shakePower_{ 0.15f,0.15f };

};