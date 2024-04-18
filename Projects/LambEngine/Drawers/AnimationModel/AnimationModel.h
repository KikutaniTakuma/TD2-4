//#pragma once
//#include "../Model/Model.h"
//#include "Engine/Graphics/Animator/Animator.h"
//
//#include <memory>
//
//class AnimationModel : public Model {
//public:
//	AnimationModel() = default;
//	AnimationModel(const std::string& fileName);
//	AnimationModel(const AnimationModel&) = default;
//	AnimationModel(AnimationModel&&) = default;
//	virtual ~AnimationModel() = default;
//
//	AnimationModel& operator=(const AnimationModel&) = default;
//	AnimationModel& operator=(AnimationModel&&) = default;
//
//public:
//	virtual void Load(const std::string& fileName) override;
//
//	virtual void Update();
//
//	virtual void Draw(const Mat4x4& camera, BlendType blend, bool isLighting) override;
//
//public:
//	inline Animator& GetAnimator() {
//		return *animator_;
//	}
//
//private:
//	void Draw(const Mat4x4& camera, BlendType blend) override;
//
//private:
//	std::unique_ptr<Animator> animator_;
//};