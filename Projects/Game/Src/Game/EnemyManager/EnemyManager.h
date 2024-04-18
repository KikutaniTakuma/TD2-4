#pragma once
#include"Game/Enemy/Enemy.h"
#include"Game/GameManager/GameManager.h"
class EnemyManager{
private:
	EnemyManager() = default;
	EnemyManager(const EnemyManager&) = delete;
	EnemyManager(EnemyManager&&) = delete;
	~EnemyManager();

	EnemyManager& operator=(const EnemyManager&) = delete;
	EnemyManager& operator=(EnemyManager&&) = delete;	

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 解放
	/// </summary>
	void Finalize();

	static EnemyManager* const GetInstance() {
		static EnemyManager instance;
		
		return &instance;
	}

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(const Camera& camera);

	/// <summary>
	/// imguiの表示
	/// </summary>
	void Debug();

	/// <summary>
	/// 箱を追加する
	/// </summary>
	/// <param name="pos">座標</param>
	void AddEnemy(const Vector3& pos);

	/// <summary>
	/// 敵を削除する
	/// </summary>
	/// <param name="pos"> 座標</param>
	void DeadEnemy(const Vector3& pos);

public:
	Vector3 GetEnemyStartPos(const size_t i)const { return startPosList_[i]; }

	size_t GetStartPosListSize()const { return startPosList_.size(); }

	void ListReset();
private:
	Map* map_ = nullptr;

	Map::MapSize* mapSize_ = nullptr;

	Input* input_ = nullptr;

	// 箱の数
	size_t EnemyCount_{};

	// 箱の数
	size_t ListCount_{};

	float moveSpeed_ = 1.000002f;
	//箱リスト
	std::list<std::unique_ptr<Enemy>> EnemyList_;

	std::vector<Vector3> startPosList_;

	std::unique_ptr<Enemy> newEnemy_;
};

