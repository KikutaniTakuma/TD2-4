#pragma once
#include "../GameObject.h"
#include "LocalBodyComp.h"
#include "LocalRigidbody.h"
#include "LocalRigidbody.h"

class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() = default;

	void Init() override;

	void Update() override;


private:
	void Input();

	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;
	Lamb::SafePtr<LocalRigidbody> pLocalRigidbody_ = nullptr;

};