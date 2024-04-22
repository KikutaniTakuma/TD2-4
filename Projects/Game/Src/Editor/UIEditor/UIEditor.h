#pragma once
#include"Drawers/Texture2D/Texture2D.h"
#include"Game/GameManager/GameManager.h"
#include"Scenes/Manager/SceneManager.h"

class UIEditor{
private:
	UIEditor() = default;
	UIEditor(const UIEditor&) = delete;
	UIEditor(UIEditor&&) = delete;
	~UIEditor();

	UIEditor& operator=(const UIEditor&) = delete;
	UIEditor& operator=(UIEditor&&) = delete;
public:
	static UIEditor* const GetInstance() {
		static UIEditor instance;

		return &instance;
	}

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

public:
	//std::vector<Tex2DState*> GetTexStateTitle() const{
	//	/*std::vector<Tex2DState*> texStateTitle;

	//	for (auto& vec : texies_[0]) {
	//		for (auto& ptr : vec[0]) {
	//			texStateTitle.push_back(ptr.get());
	//		}
	//	}

	//	return texStateTitle;*/
	//}
	//std::vector<std::unique_ptr<Tex2DState>> GetTexStateSelect() const{
	//	return texies_[1];
	//}
	//std::vector<std::unique_ptr<Tex2DState>> GetTexStateGame() const{
	//	return texies_[2];
	//}
	//std::vector<std::unique_ptr<Tex2DState>> GetTexStateResult()const {
	//	return texies_[3];
	//}

private:
	Input* input_ = nullptr;

	Texture2D* tex2D_ = nullptr;

	std::unordered_map<BaseScene::ID, std::string> sceneName_;

	/// <summary>
	/// 一時保存するリスト
	/// </summary>
	std::array<std::vector<std::unique_ptr<Tex2DState>>, BaseScene::maxScene_> texies_;

	std::unique_ptr<Tex2DState> newTex_;

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

	//imguiの操作をそのまま続けるかどうかのメッセージボックスを表示
	bool OperationConfirmation();

private:
	//jsonの数値をVector3に変換する関数
	void from_json(const json& j, Vector2& v);

private:
	//ファイル保存関連
	//ファイルがちゃんと読み込めたかどうか
	bool chackOnlyNumber_ = 0;
	//ステージ名をいれるコンテナ
	std::vector<std::string> stages_;
	//選んでいるステージ名
	std::string stageName_;
	//アイテムのファイルパス
	inline static const std::string kDirectoryPath_ = "Resources/Datas/UIs/";
	//アイテムのファイルパス
	inline static const std::string kDirectoryName_ = "Resources/Datas/UIs";
	//名前
	inline static const std::string kItemName_ = "UIs";

	std::vector<std::string> fileName_;

	

};

