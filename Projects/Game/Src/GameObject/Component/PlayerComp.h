#pragma once
#include "../GameObject.h"

class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() = default;

	void Init() override;

	void Start() override;

	void Update() override;

private:

	

};
