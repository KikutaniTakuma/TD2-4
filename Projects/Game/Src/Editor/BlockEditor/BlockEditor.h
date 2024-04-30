#pragma once
#include"Game/GameManager/GameManager.h"
#include"Game/CollisionManager/Obb/Obb.h"
class BlockEditor{

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

	//ファイルを読み込む(走査)
	void LoadFiles(const std::string& fileName);

	//ファイルを読み込む(走査)
	void LoadFiles(const int32_t selectNum);

	/// <summary>
	/// マウスのポジションを3D空間上に変換する
	/// </summary>
	void MousePosTrans(const Camera& camera);
private:
	/// <summary>
	/// 前回ロード時の状態に戻す
	/// </summary>
	void DataReset();

	/// <summary>
	/// 一層だけ前回ロード時の状態に戻す
	/// </summary>
	void FloorReset();

	/// <summary>
	/// 完全に白紙の状態に戻す
	/// </summary>
	void AllDataReset();

	//imguiの操作をそのまま続けるかどうかのメッセージボックスを表示
	bool OperationConfirmation();

	/// <summary>
	/// マップの中にpointがあるかどうか
	/// </summary>
	bool MapinPoint(const Vector3& point);

	

private:

	GroundEditor* map_ = nullptr;

	GroundEditor::MapSize * mapSize_ = nullptr;

	std::array<size_t, 3> boxPos_;

	std::array<bool, GroundEditor::kMapX> boxData_;

	Input* input_ = nullptr;

	//設置するするとき用のプリミティブ
	std::array< std::unique_ptr<Obb>, 3> obb_;

	// 自機から3Dレティクルへの距離
	float distancePlayerTo3DReticleCopy_ = 30.0f;
	//本来図る距離とは変えるための変数
	float correction_ = 5.0f;

	bool isUseImgui_ = false;

	std::unique_ptr<Obb> reticle_;

	std::unique_ptr<Obb> scanningOBB_;

	/*ファイル制御関連*/
private:
	//メンバ関数
	using json = nlohmann::json;
	//ファイルに保存する
	void SaveFile(const std::string& fileName);
	//ファイルが存在するか確認する
	void ChackFiles();
	
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
	inline static const std::string kDirectoryPath_ = "Resources/Datas/Boxes/";
	//アイテムのファイルパス
	inline static const std::string kDirectoryName_ = "Resources/Datas/Boxes";
	//名前
	inline static const std::string kItemName_ = "boxes";

	std::vector<std::string> fileName_;

};

