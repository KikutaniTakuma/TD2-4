#pragma once
#include "../GameObject.h"
#include "SpriteComp.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Engine/Graphics/Tex2DAniamtor/Tex2DAniamtor.h"

class SpriteAnimatorComp : public IComponent
{
public:
	using IComponent::IComponent;
	~SpriteAnimatorComp() = default;

	void Init() override;

	void Update() override;

	Tex2DAniamtor *GetAnimator() { return textureAnimator_.get(); }

private:

	std::unique_ptr<Tex2DAniamtor> textureAnimator_;

	Lamb::SafePtr<SpriteComp> pSpriteComp_ = nullptr;

};
