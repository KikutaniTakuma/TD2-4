#pragma once
#include"Game/Block/Block.h"
#include"Game/CollisionManager/CollisionManager.h"
#include"Input/Input.h"
#include"Game/CollisionManager/Obb/Obb.h"
class Player{
public:
	~Player();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(const Camera& camera);

	void PlayerMove();

	/// <summary>
	/// imguiの表示
	/// </summary>
	void Debug();

	/// <summary>
	/// 箱を追加する
	/// </summary>
	/// <param name="pos">座標</param>
	void AddBox(const Vector3& pos);

	/// <summary>
	/// 交換するだけの関数
	/// </summary>
	void AllTrade();

private:
	CollisionManager* pCollisionManager_ = nullptr;

	Input* input_ = nullptr;
	//場所を示すための
	std::unique_ptr<Obb> obb_;
	
	// 箱の数
	size_t boxCount_{};

	float moveSpeed_ = 1.000002f;
	//箱リスト
	std::list<std::unique_ptr<Block>> blockList_;

	std::unique_ptr<Block> newBlock_;
};

