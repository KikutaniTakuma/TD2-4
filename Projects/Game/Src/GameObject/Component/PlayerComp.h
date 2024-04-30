#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"

#include "Game/Map/Map.h"
#include "Game/GameManager/GameManager.h"


class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() = default;

	void Init() override;

	void Start() override;

	void Update() override;

	void SetMap(Map *map);

	void SetGameManager(GameManager *pGameManager);

public:

	void MoveInput(int32_t xMove);

private:

	void SpawnFallingBlock();


private:

	// ローカルの X座標
	int32_t localPosX_;

	// マップの参照
	Lamb::SafePtr<Map> pMap_;

	// ゲームマネージャへの参照
	Lamb::SafePtr<GameManager> pGameManager_;

};
