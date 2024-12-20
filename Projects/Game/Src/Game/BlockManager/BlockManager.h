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
#include "Game/CollisionManager/Obb/Obb.h"
#include <json.hpp>


class BlockManager{
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
	void AddBlock(const Vector3& pos, const Vector3& scale);

	//imguiの操作をそのまま続けるかどうかのメッセージボックスを表示
	bool OperationConfirmation();

private:
	/*std::list<std::unique_ptr<GameObject>> itemList_;

	std::unique_ptr<GameObject> testObject_;*/
	//モデルのコンテナ
	std::unordered_map<Vector3, std::unique_ptr<Model>> modelList_;

	std::unique_ptr<Model> model_;

	std::unique_ptr<Model> model2_;
	//設置するするとき用のプリミティブスフィア
	std::unique_ptr<Obb> obb_;

	float boxSize_ = 1.0f;

	//元の色
	const uint32_t baseColor_ = 0xffffffff;
	//変更後の色
	const uint32_t moveColor_ = 0xff0000ff;
	//大きさをimguiで変更しているかどうか
	bool changeSize_ = false;

	//そのimguiを選択しているかどうか
	bool isSelectImgui_ = false;
	

	/*ファイル制御関連*/
private:
	//メンバ関数
	using json = nlohmann::json;
	//ファイルに保存する
	void SaveFile(const std::string& fileName);
	//ファイルが存在するか確認する
	void ChackFiles();
	//ファイルを読み込む(走査)
	void LoadFiles(const std::string& fileName);
	//ファイルを読み込む
	void LoadFile(const std::string& fileName);
	//ファイルが存在するか確認する(指定)
	bool LoadChackItem(const std::string& fileName);

private:
	//jsonの数値をVector3に変換する関数
	void from_json(const json& j, Vector3& v);

private:
	//ファイル保存関連
	//ファイルがちゃんと読み込めたかどうか
	bool chackOnlyNumber_ = 0;
	//ステージ名をいれるコンテナ
	std::vector<std::string> stages_;
	//選んでいるステージ名
	std::string stageName_;
	//imguiで選択しているステージナンバー
	int stageSelectNum_;
	//アイテムのファイルパス
	const std::string kDirectoryPath_ = "Resources/Datas/Blocks/";
	//アイテムのファイルパス
	const std::string kDirectoryName_ = "Resources/Datas/Blocks";
	//名前
	const std::string kItemName_ = "block";

	std::vector<std::string> fileName_;

private:
	//ステージ名をいれるコンテナ
	std::vector<std::string> shapes_;
	//選んでいるステージ名
	std::string shapeName_;
	//imguiで選択しているステージナンバー
	int shapeSelectNum_;

	//ペアのファイルパス
	const std::string kDirectoryPairPath_ = "Resources/Datas/Pairs/";
	//ペアのファイルパス
	const std::string kDirectoryPairName_ = "Resources/Datas/Pairs";
	//名前
	const std::string kPairName_ = "shape";

	std::vector<std::string> pairFileName_;

private:
	//ファイルに保存する
	//void SaveFilePair(const std::string& fileName);
	//ファイルを読み込む
	//void LoadFilePair(const std::string& fileName);
};

