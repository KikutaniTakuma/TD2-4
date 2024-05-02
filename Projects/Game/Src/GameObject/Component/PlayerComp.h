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

	/// @brief 接地できる横幅の最大値を取得する
	/// @return 横幅
	int32_t GetMaxBlockWidth() const;

	void SetGauge(BlockGauge *pBlockGauge);

private:

	int32_t startPos_ = -1;

	Model *fallBlockModel_;

	inline static constexpr int32_t kMaxWidth_ = 3;

private:

	Lamb::SafePtr<BlockGauge> pBlockGauge_ = nullptr;

	Lamb::SafePtr<LocalBodyComp> pLocalPosComp_ = nullptr;

};
