#pragma once
#include<list>
#include "GameObject/GameObject.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/CollisionManager/Collider/Collider.h"
#include "Game/CollisionManager/Sphere/Sphere.h"


class ItemManager{
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
	void Draw(const Camera* camera);

	/// <summary>
	/// Imguiの情報
	/// </summary>
	void Debug();

	/// <summary>
	///	リストの終端に追加する
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="scale">大きさ</param>
	void AddItem(const Vector3& pos, const Vector3& scale);

private:
	/*std::list<std::unique_ptr<GameObject>> itemList_;

	std::unique_ptr<GameObject> testObject_;*/

	std::list<std::unique_ptr<Model>> modelList_;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Model> model2_;

	std::unique_ptr<Sphere> sphere_;

};

