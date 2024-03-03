#pragma once
#include <list>
#include "../SoLib/Containers/Singleton.h"

/// <summary>
/// コリジョンマネージャー
/// </summary>
class CollisionManager : public SoLib::Singleton<CollisionManager> {
	friend SoLib::Singleton<CollisionManager>;

// public:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager(CollisionManager&&) = delete;
	~CollisionManager() = default;

	CollisionManager& operator=(const CollisionManager&) = delete;
	CollisionManager& operator=(CollisionManager&&) = delete;

public:
	void SetCollider(class Collider* collider);

	void Update();

private:
	std::list<class Collider*> colliders_;
};