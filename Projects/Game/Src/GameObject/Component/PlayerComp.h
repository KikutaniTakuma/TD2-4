#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"

#include "Game/Map/Map.h"
#include "Game/GameManager/GameManager.h"
#include "LocalBodyComp.h"
#include "Drawers/Model/Model.h"


class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() = default;

	void Init() override;

	void Start() override;

	void Update() override;

	void Draw(const Camera &camera) const override;

public:

	void MoveInput(int32_t xMove);

	void SpawnFallingBlock();

	void SetStartPos();

private:

	int32_t startPos_ = -1;

	Model *fallBlockModel_;


private:

	Lamb::SafePtr<LocalBodyComp> pLocalPosComp_ = nullptr;

};