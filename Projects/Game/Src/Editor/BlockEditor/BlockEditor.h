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

	Map* map_ = nullptr;

	Map::MapSize* mapSize_ = nullptr;

	//設置するするとき用のプリミティブ
	std::unique_ptr<Obb> obb_;

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
	const std::string kDirectoryPath_ = "Resources/Datas/Boxes/";
	//アイテムのファイルパス
	const std::string kDirectoryName_ = "Resources/Datas/Boxes";
	//名前
	const std::string kItemName_ = "box";

	std::vector<std::string> fileName_;

};

