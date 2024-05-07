#pragma once
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
	void Update(float deltaTime);

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
	BlockMap* map_ = nullptr;

	BlockMap::Block2dMap * mapSize_ = nullptr;

	Input* input_ = nullptr;

	float moveSpeed_ = 1.000002f;
	//箱リスト
	std::list<std::unique_ptr<GameObject>> enemyList_;

	std::vector<Vector3> startPosList_;

	std::unique_ptr<GameObject> newEnemy_;
};

