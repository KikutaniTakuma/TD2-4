#include "GameManager.h"
#include "GameObject/Component/IvyComponent.h"

void GameManager::Init() {
	mainIvy_ = std::make_unique<GameObject>();
	mainIvy_->AddComponent<IvyComponent>();
}

void GameManager::Update([[maybe_unused]] const float deltaTime) {
	mainIvy_->Update(deltaTime);
}

void GameManager::Draw([[maybe_unused]] const Camera &camera) const {
	mainIvy_->Draw(camera);
}
