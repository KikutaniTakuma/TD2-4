#pragma once
#include "../GameObject.h"

class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() = default;

	void Init() const;

private:

};
