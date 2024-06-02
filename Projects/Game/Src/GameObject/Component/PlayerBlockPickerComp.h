#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"
#include "LocalBodyComp.h"

class PlayerBlockPickerComp : public IComponent {
public:
	using IComponent::IComponent;

	void Init() override;

	void Update() override;

	void Draw(const Camera &camera) const override;

	void PickUp(int32_t facing);

	void Drop(int32_t facing);

	bool IsPicking() const { return pickingBlock_ or not dwarfList_.empty(); }

	const Mat4x4 &GetBlockAffine() const { return affine_; }

	int32_t GetFacing() const {
		return facing_;
	}

private:

	inline static Lamb::SafePtr<Texture2D> pTexture_ = nullptr;
	inline static uint32_t texID_ = 0;
	Mat4x4 affine_;
	Mat4x4 targetAffine_;
	bool targetIsBlock_;

	Block pickingBlock_ = Block::BlockType::kNone;

	uint32_t targetFlameTex_;

	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;
	Lamb::SafePtr<BlockMap> pBlockMap_ = nullptr;

	std::list<GameManager::DwarfPick> dwarfList_;

	int32_t facing_;

	Audio* noSpace_ = nullptr;
	Audio* pickUp_ = nullptr;

	Audio* putBlock_ = nullptr;


};