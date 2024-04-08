#pragma once
#include <list>
#include "../SoLib/Containers/Singleton.h"
#include"Game/CollisionManager/Collider/Collider.h"

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
	void SetCollider(Collider* collider);

	void DeleteCollider(Collider* collider);

	void Debug();

	void Update();

private:
	std::list<Collider*> colliders_;
};