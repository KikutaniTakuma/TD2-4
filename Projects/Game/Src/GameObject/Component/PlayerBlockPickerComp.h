#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"
#include "LocalBodyComp.h"

class PleyerBlockPickerComp : public IComponent {
public:
	using IComponent::IComponent;

	void Init() override;

	void Update() override;

	void Draw(const Camera &camera) const override;

	void PickUp(int32_t facing);

	void Drop(int32_t facing);

	bool IsPicking() const { return pickingBlock_; }

private:

	inline static Lamb::SafePtr<Texture2D> pTexture_ = nullptr;
	inline static uint32_t texID_ = 0;
	Mat4x4 affine_;

	Block pickingBlock_ = Block::BlockType::kNone;

	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;
	Lamb::SafePtr<BlockMap> pBlockMap_ = nullptr;

};