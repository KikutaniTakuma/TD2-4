#pragma once
#include"Game/Block/Block.h"
#include"Game/CollisionManager/CollisionManager.h"
#include"Input/Input.h"
#include"Game/CollisionManager/Obb/Obb.h"

#include "Utils/SafePtr/SafePtr.h"

class Enemy{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();

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

	/// <summary>
	/// imguiの表示
	/// </summary>
	void Debug(const size_t name);

public:
	void SetDead() { isDead_ = true; }

	void SetStartPos(const Vector3& pos) { startPos_ = pos; }

	bool GetIsDead()const { return isDead_; }

	Obb* GetObb()const { return obb_.get(); }

	const Vector3& GetPos()const { return transform.translate; }

	const Vector3& GetStartPos()const { return startPos_; }

public:
	Transform transform;
private:
	Lamb::SafePtr<Model> model_;

	std::unique_ptr<Obb> obb_;

	bool isDead_;

	Vector3 startPos_;

};

