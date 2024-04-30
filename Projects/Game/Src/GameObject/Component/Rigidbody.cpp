#include "Rigidbody.h"
#include "Collider.h"
#include <Game/CollisionManager/CollisionManager.h>

void Rigidbody::Init() {
	velocity_ = {};
	acceleration_ = {};
}

void Rigidbody::Update() {

	static CollisionManager* const collisionManager = CollisionManager::GetInstance();

	beforePos_ = transform_.translate;

	velocity_ += acceleration_;

	transform_.translate += velocity_ * GetDeltaTime();

	acceleration_ = {};
}
