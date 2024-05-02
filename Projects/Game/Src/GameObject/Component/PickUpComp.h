#pragma once
#include "../GameObject.h"
#include "Drawers/Model/Model.h"
#include "LocalBodyComp.h"

struct PickUpBlockData;
class DwarfComp;

class PickUpComp :public IComponent
{
public:

	inline static const char *const kModelName_ = "Resources/Cube.obj";
	static constexpr int32_t kMaxWeight_ = 6;

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

	const std::list<PickUpBlockData> &GetPickUpBlockList() const { return pickupBlockList_; }

	void PopFront() { pickupBlockList_.pop_front(); }

	void ThrowAllBlocks();

	void AddBlock(PickUpBlockData blockData) { pickupBlockList_.push_back(blockData); }

private:

	Model *model_;

	std::list<PickUpBlockData> pickupBlockList_;
	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;

	DwarfComp *pDwarfComp_ = nullptr;
};