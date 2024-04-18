#include "CollisionManager.h"
#include "Collider/Collider.h"
#include<imgui.h>

void CollisionManager::SetCollider(class Collider* collider) {
	colliders_.push_back(collider);
}

void CollisionManager::SetCollider(Obb* collider){
	obbColliders_.push_back(collider);
}

void CollisionManager::DeleteCollider(class Collider* collider){
	auto it = std::find(colliders_.begin(), colliders_.end(), collider);
	if (it != colliders_.end()) {
		colliders_.erase(it);
	}
}

void CollisionManager::DeleteCollider(Obb* collider){
	auto it = std::find(obbColliders_.begin(), obbColliders_.end(), collider);
	if (it != obbColliders_.end()) {
		obbColliders_.erase(it);
	}
}

void CollisionManager::Debug(){
	ImGui::Begin("コライダーリスト");
	int j = 0;
	for (auto i = obbColliders_.begin(); i != obbColliders_.end(); i++){
		ImGui::DragFloat3(("ポジション" + std::to_string(j)).c_str(), &(*i)->center_.x, 0.1f);
		ImGui::Text(("スケール x:%.2f y:%.2f z:%.2f" + std::to_string(j)).c_str(), (*i)->scale_.x, (*i)->scale_.y, (*i)->scale_.z);
		j++;
	}

	ImGui::End();
}

void CollisionManager::Update() {
	for (auto i = obbColliders_.begin(); i != obbColliders_.end();i++) {
		for (auto j = obbColliders_.begin(); j != obbColliders_.end(); j++) {
			if (i == j) {
				continue;
			}

			if ((*i)->IsCollisionOBBOBB(**j)) {
				break;
			}
		}
	}
}

