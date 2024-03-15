#pragma once
#include "../GameObject.h"

#include <memory>

#include "../LambEngine/Drawers/Texture2D/Texture2D.h"

class EnergyItem :public IComponent {
public:
	using IComponent::IComponent;

	void Init() override;

	void Update() override;

	void Draw(const Camera &camera) const override;

	float GetRadius() const { return vRadius_; }

private:

	std::unique_ptr<Texture2D> itemTexture_;

	SoLib::VItem<"当たり判定の半径", float> vRadius_ = 10.f;

};