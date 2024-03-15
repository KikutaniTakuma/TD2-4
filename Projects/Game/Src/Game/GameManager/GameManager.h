#pragma once
#include "../SoLib/Containers/Singleton.h"

#include <memory>
#include <bitset>
#include <list>

#include "../LambEngine/Input/Input.h"

#include "GameObject/GameObject.h"

class GameManager : public SoLib::Singleton<GameManager> {
private:

	friend SoLib::Singleton<GameManager>;
	GameManager() = default;
	GameManager(const GameManager &) = delete;
	GameManager &operator=(const GameManager &) = delete;
	~GameManager() = default;

public:

	void Init();

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;

public:

	bool Debug(const char *const str);

public:

	/// @brief 入力動作
	void InputAction();

	/// @brief 栄養の生成
	/// @param count 生成数
	void RandomPopEnergys(const Vector2 &origin, const Vector2 &radius, size_t count);

private:

	/// @brief ツルの追加
	/// @return 追加されたツルのオブジェクト
	GameObject *RandomAddIvy();

	/// @brief ツルの追加
	/// @return 追加されたツルのオブジェクト
	GameObject *AddIvy(uint32_t index);

	/// @brief ツルの追加
	/// @return 追加されたツルのオブジェクト
	GameObject *AddIvy(const Vector3 &pos, uint32_t index);

	/// @brief ツルの追加
	/// @return 追加されたツルのオブジェクト
	GameObject *AddEnergy(const Vector3 &pos = {});

private:

	SoLib::VItem<"中心地", Vector2> centor_{ { 0, 300 } };
	SoLib::VItem<"半径", Vector2> radius_{ { 150, 150 } };
	SoLib::VItem<"生成数", int32_t> count_{ 1 };

private:

	SoLib::VItem<"ツタの間隔", float> vIvyDistance_ = 100.f;

private:
	// 入力マネージャ
	Input *input_ = nullptr;

	// ツタの生える位置
	std::bitset<6u> ivyPos_;

	// ツタ
	std::list<std::unique_ptr<GameObject>> ivys_;
	// 栄養アイテム
	std::list<std::unique_ptr<GameObject>> energyItems_;

};