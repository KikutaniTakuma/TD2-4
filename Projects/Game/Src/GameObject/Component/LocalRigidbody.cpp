#include "LocalRigidbody.h"

void LocalRigidbody::Init()
{
	velocity_ = {};
	acceleration_ = {};
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
}

void LocalRigidbody::Update()
{

	const float deltaTime = GetDeltaTime();

	beforePos_ = pLocalBodyComp_->localPos_;

	acceleration_ -= velocity_ * (deltaTime * friction_);

	velocity_ += acceleration_;

	pLocalBodyComp_->localPos_ += velocity_ * deltaTime;

	acceleration_ = {};
}
