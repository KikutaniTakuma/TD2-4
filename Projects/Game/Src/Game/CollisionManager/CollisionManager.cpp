#include "CollisionManager.h"
#include "Collider/Collider.h"
#include<imgui.h>

void CollisionManager::SetCollider(class Collider* collider) {
	colliders_.push_back(collider);
}

void CollisionManager::DeleteCollider(class Collider* collider){
	auto it = std::find(colliders_.begin(), colliders_.end(), collider);
	if (it != colliders_.end()) {
		colliders_.erase(it);
	}
}

void CollisionManager::Debug(){
	ImGui::Begin("コライダーリスト");
	int j = 0;
	for (auto i = colliders_.begin(); i != colliders_.end(); i++){
		ImGui::DragFloat3(("ポジション" + std::to_string(j)).c_str(), &(*i)->collisionPos_.x, 0.1f);
		ImGui::Text(("スケールmin x:%.2f y:%.2f z:%.2f" + std::to_string(j)).c_str(), (*i)->GetMin().x, (*i)->GetMin().y, (*i)->GetMin().z);
		ImGui::Text(("スケールmax x:%.2f y:%.2f z:%.2f" + std::to_string(j)).c_str(), (*i)->GetMax().x, (*i)->GetMax().y, (*i)->GetMax().z);
		j++;
	}

	ImGui::End();
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

