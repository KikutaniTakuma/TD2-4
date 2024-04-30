#pragma once
#include "../GameObject.h"
#include "Drawers/Model/Model.h"

struct PickUpBlockData;

class PickUpComp :public IComponent
{
public:

	inline static const char *const kModelName_ = "Resources/Cube.obj";

public:
	using IComponent::IComponent;
	~PickUpComp() = default;

	void Init() override;

	void Draw(const Camera &camera) const override;

public:

	/// @brief 持っているブロックの重さを返す
	/// @return ブロックの重さ
	int32_t GetBlockWeight() const;

	bool PickupTargetBlock(Vector2 localPos);

private:

	Model *model_;

	std::list<PickUpBlockData> pickupBlockList_;


};