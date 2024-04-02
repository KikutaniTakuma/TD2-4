#pragma once
#include "../SoLib/Containers/Singleton.h"
#include "../SoLib/Containers/Array2D.h"

#include <memory>
#include <bitset>
#include <list>
#include <cstdint>

#include "../LambEngine/Input/Input.h"

#include "GameObject/GameObject.h"
#include <Drawers/Model/Model.h>
#include <Game/CollisionManager/AABB/AABB.h>

class IvyComponent;

class GameManager : public SoLib::Singleton<GameManager> {
private:

	friend SoLib::Singleton<GameManager>;
	GameManager() = default;
	GameManager(const GameManager &) = delete;
	GameManager &operator=(const GameManager &) = delete;
	~GameManager() = default;

public:

	enum class BoxType : uint32_t {
		kNone,	// 虚空
		kBox,	// 箱
	};

public:

	void Init();

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;

public:

	bool Debug(const char *const str);

public:

	/// @brief 入力動作
	void InputAction();

private:

	void BoxDraw(const Camera &camera) const;

	/// @brief 箱のデータをモデルに転送
	void TransferBoxData();

private:
	// 入力マネージャ
	Input *input_ = nullptr;
	std::list<std::unique_ptr<Model>> models_;

	// 箱の配列 [y][z][x]
	std::array<std::array<std::array<BoxType, 10u>, 10>, 3u> boxMap_;
	// 箱の数
	size_t boxCount_;

	SoLib::VItem<"ブロックの間隔", Vector2> vBoxDistance_{ {1, 3} };

	Vector3 playerPos_;

};