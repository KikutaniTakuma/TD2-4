#pragma once
#include "Game/Map/Map.h"
#include "Utils/SafePtr/SafePtr.h"

// ゲームマネージャ
class GameManager;	// シングルトン。前方宣言なのでSafePtrは使えない。

class GameEffectManager {
public:
	GameEffectManager() = default;
	~GameEffectManager() = default;

	void Init();

	void Update(float deltaTime);

	void Draw(const Camera &camera) const;

private:

	// ゲームマネージャの参照。
	GameManager *pGameManager_ = nullptr;

	Lamb::SafePtr<Map> pMap_ = nullptr;

};