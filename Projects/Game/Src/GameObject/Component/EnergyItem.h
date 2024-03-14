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

private:

	std::unique_ptr<Texture2D> itemTexture_;

};