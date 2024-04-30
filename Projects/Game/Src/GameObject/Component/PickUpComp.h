#pragma once
#include "../GameObject.h"

struct PickUpBlockData;

class PickUpComp :public IComponent
{
public:
	using IComponent::IComponent;
	~PickUpComp() = default;

public:

	/// @brief 持っているブロックの重さを返す
	/// @return ブロックの重さ
	int32_t GetBlockWeight() const;

	bool PickupTargetBlock(Vector2 localPos);

private:

	std::list<PickUpBlockData> pickupBlockList_;


};