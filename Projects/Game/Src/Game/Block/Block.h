#pragma once
#include <array>
#include <cstdint>
#include "../SoLib/Containers/VItem.h"
#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include <list>
#include <Drawers/Model/Model.h>
#include <Camera/Camera.h>
#include"Game/CollisionManager/AABB/AABB.h"
#include"Game/CollisionManager/Collider/Collider.h"
#include <bitset>
#include <imgui.h>
class Block{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(const Camera& camera);

	/// <summary>
	/// Imguiの情報
	/// </summary>
	void Debug();

	/// <summary>
	/// 交換するだけの関数
	/// </summary>
	void Trade();

public:
	bool GetIsBreak()const { return isBreak_; }

	Collider* GetCollider()const { return collider_.get(); }

public:
	Vector3 pos_;

	float rotateZ_ = 0.0f;

private:
	inline static SoLib::VItem<"ブロックのサイズ", Vector2> vBlockScale{ {1.5f ,0.5f} };

	std::unique_ptr<Model> model_;

	std::unique_ptr<Collider> collider_;

	bool isBreak_;
};

