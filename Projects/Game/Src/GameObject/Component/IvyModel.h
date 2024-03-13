#pragma once
#include "../GameObject.h"

#include <memory>
#include <list>

#include "../LambEngine/Drawers/Line/Line.h"

class IvyModelComponent :public IComponent {
public:
	using IComponent::IComponent;

	~IvyModelComponent() = default;

	void Init() override;

	void Draw(const Camera &camera) const override;


private:

	std::list<std::unique_ptr<Line>> lines_;
};
