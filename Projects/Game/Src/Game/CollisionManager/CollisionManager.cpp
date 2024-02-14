#include "CollisionManager.h"
#include "Collider/Collider.h"

void CollisionManager::SetCollider(class Collider* collider) {
	colliders_.push_back(collider);
}

void CollisionManager::Update() {
	for (auto i = colliders_.begin(); i != colliders_.end();i++) {
		for (auto j = colliders_.begin(); j != colliders_.end(); j++) {
			if (i == j) {
				continue;
			}
			else {
				(*i)->CollisionExtrusion(**j);
			}
		}
	}
}

