#pragma once
#include "../SoLib/Containers/Singleton.h"

#include <memory>
#include <bitset>
#include <list>

#include "../LambEngine/Input/Input.h"

#include "GameObject/GameObject.h"

class IvyComponent;

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

	/// @brief ツタの分裂
	/// @return 分裂に成功したか
	bool IvySprit();

	/// @brief ツタが最大値を超えたら破棄
	void DeleteIvyMaximum();

	/// @brief 動作中のツタが動いているか
	/// @return 動いている場合true
	bool CurrentIvyIsActive() const;

private:
	/// @brief ツタを破棄する
	/// @param ivy ツタのアドレス
	void DeleteIvy(GameObject *ivy);

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
	GameObject *AddEnergy(const Vector3 &pos);

	/// @brief 栄養を回収する
	/// @param energy 回収する栄養
	void CollectEnergy(GameObject *energy, IvyComponent *ivy);

	/// @brief ツタの分裂数が増えるかを計算
	void AddIvySplitCount(IvyComponent *ivy);

private:

	SoLib::VItem<"中心地", Vector2> centor_{ { 0, 300 } };
	SoLib::VItem<"半径", Vector2> radius_{ { 150, 150 } };
	SoLib::VItem<"生成数", int32_t> count_{ 1 };

private:

	SoLib::VItem<"ツタの間隔", float> vIvyDistance_ = 100.f;
	SoLib::VItem<"ツタの初期分裂数", uint32_t> vDefaultIvySplit_ = 3u;

	// 一度に生やせるツタの最大値
	inline static constexpr uint32_t maxIvyCount_ = 3u;

private:
	// 入力マネージャ
	Input *input_ = nullptr;

	// ツタの生える位置
	std::bitset<6u> ivyPos_;

	// 現在の分裂が持っているツタの数
	std::list<GameObject *> currentCollectedEnergyItems_;
	// ツタ全体が持っているツタの数
	std::list<std::unique_ptr<GameObject>> collectedEnergyItems_;

	// ツタの分裂数
	uint32_t ivySplit_ = 3u;
	// ツタ
	std::list<std::unique_ptr<GameObject>> ivys_;
	// 栄養アイテム
	std::list<std::unique_ptr<GameObject>> energyItems_;

};