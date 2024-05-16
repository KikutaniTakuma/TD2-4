#pragma once
#include"Drawers/Texture2D/Texture2D.h"
#include"Game/GameManager/GameManager.h"
#include"Scenes/Manager/BaseScene/BaseScene.h"
#include"Engine/Graphics/Tex2DAniamtor/Tex2DAniamtor.h"

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
	void Update(const BaseScene::ID id);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(const Mat4x4& camera, const BaseScene::ID id);

	/// <summary>
	/// 設置位置描画処理
	/// </summary>
	void PutDraw(const Mat4x4& camera);

	/// <summary>
	/// Imguiの情報
	/// </summary>
	void Debug(const BaseScene::ID id);

	
	//全てのファイルを読み込む
	void LoadFileAll();

	/*シーンごとのテクスチャを取得する関数*/

	std::vector<Tex2DState*> GetTitleTextures() {
		std::vector<Tex2DState*> texStateTitle;
		for (std::unique_ptr<Tex2DState>& ptr : texies_[1]) {
			texStateTitle.push_back(ptr.get());
		}
		return texStateTitle;
	}

	std::vector<Tex2DState*> GetGameTextures() {
		std::vector<Tex2DState*> texStateGame;
		for (std::unique_ptr<Tex2DState>& ptr : texies_[3]) {
			texStateGame.push_back(ptr.get());
		}
		return texStateGame;
	}

	std::vector<Tex2DState*> GetSelectTextures() {
		std::vector<Tex2DState*> texStateSelect;
		for (std::unique_ptr<Tex2DState>& ptr : texies_[2]) {
			texStateSelect.push_back(ptr.get());
		}
		return texStateSelect;
	}

	std::vector<Tex2DState*> GetResultTextures() {
		std::vector<Tex2DState*> texStateResult;
		for (std::unique_ptr<Tex2DState>& ptr : texies_[0]) {
			texStateResult.push_back(ptr.get());
		}
		return texStateResult;
	}

private:
	Input* input_ = nullptr;

	Texture2D* tex2D_ = nullptr;

	//std::unique_ptr<Tex2DAniamtor> texAnim_;
	//uint32_t animNum_;

	std::unordered_map<BaseScene::ID, std::string> sceneName_;

	/// <summary>
	/// 一時保存するリスト
	/// </summary>
	std::array<std::vector<std::unique_ptr<Tex2DState>>, BaseScene::kMaxScene> texies_;

	std::unique_ptr<Tex2DState> newTex_;

	BaseScene::ID id_;

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

	void LoadFiles(const std::string& fileName);
	
	//ファイルが存在するか確認する(指定)
	bool LoadChackItem(const std::string& fileName);

	//imguiの操作をそのまま続けるかどうかのメッセージボックスを表示
	bool OperationConfirmation();

private:
	void from_json(const json& j, Vector2& v);

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
	//アイテムのファイルパス
	inline static const std::string kDirectoryPath_ = "Resources/Datas/UIs/";
	//アイテムのファイルパス
	inline static const std::string kDirectoryName_ = "Resources/Datas/UIs";
	//名前
	inline static const std::string kItemName_ = "UIs";

	std::vector<std::string> fileName_;

	

};

