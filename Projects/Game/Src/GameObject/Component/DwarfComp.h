#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"
#include "LocalBodyComp.h"

class DwarfComp : public IComponent
{
public:
	using IComponent::IComponent;
	~DwarfComp() = default;

	void Init() override;

	void Start() override;

	void Update() override;

public:

private:

	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;

};