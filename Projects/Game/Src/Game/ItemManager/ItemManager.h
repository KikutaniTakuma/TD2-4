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
#include <json.hpp>


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

	//imguiの操作をそのまま続けるかどうかのメッセージボックスを表示
	bool OperationConfirmation();

private:
	/*std::list<std::unique_ptr<GameObject>> itemList_;

	std::unique_ptr<GameObject> testObject_;*/
	//モデルのコンテナ
	std::list<std::unique_ptr<Model>> modelList_;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Model> model2_;
	//設置するするとき用のプリミティブスフィア
	std::unique_ptr<Sphere> sphere_;

	//元の色
	const uint32_t baseColor_ = 0xffffffff;
	//変更後の色
	const uint32_t moveColor_ = 0xff0000ff;
	//大きさをimguiで変更しているかどうか
	bool changeSize_ = false;

	/*ファイル制御関連*/
private:
	//メンバ関数
	using json = nlohmann::json;
	//ファイルに保存する
	void SaveFile(const std::string& stageNumber);
	//ファイルに書いてあるものに上書きする
	void FileOverWrite();
	//ファイルが存在するか確認する
	void ChackFiles();
	//ファイルを読み込む(走査)
	void LoadFiles(const std::string& stageNumber);
	//ファイルを読み込む
	void LoadFile(const std::string& groupName);
	//ファイルが存在するか確認する(指定)
	bool LoadChackItem(const std::string& stageNumber);

private:
	//jsonの数値をVector3に変換する関数
	void from_json(const json& j, Vector3& v);

private:
	//ファイル保存関連
	//ファイルがちゃんと読み込めたかどうか
	bool chackOnlyNumber = 0;
	//ステージ名をいれるコンテナ
	std::vector<std::string> stages_;
	//選んでいるステージ名
	std::string stageName_;
	//imguiで選択しているステージナンバー
	int stageSelectNum_;
	//アイテムのファイルパス
	const std::string kDirectoryPath = "Resources/Datas/Items/";
	//アイテムのファイルパス
	const std::string kDirectoryName = "Resources/Datas/Items";
	//名前
	const std::string kItemName_ = "nutrition";

	std::string Name_ = "\0";

	std::vector<std::string> fileName;
};

