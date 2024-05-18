#pragma once
#include"Drawers/Texture2D/Texture2D.h"
#include"Game/GameManager/GameManager.h"
#include"Scenes/Manager/BaseScene/BaseScene.h"
#include"Engine/Graphics/Tex2DAniamtor/Tex2DAniamtor.h"
#include "Game/CollisionManager/Sphere/Sphere.h"
#include "Game/CollisionManager/Collider/Collider.h"


class CatmullRomEditor{
public:

	CatmullRomEditor();

	//初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw(const Mat4x4& cameraMat);

	void Debug();


	uint32_t GetControlPointsNum() { return static_cast<uint32_t>(ControlPoints_.size()); }

	std::vector<Vector2> GetControlPoints() { return ControlPoints_; }

	Vector2 GetFirstControlPoint() { return ControlPoints_[0]; }

	uint32_t GetLastLinePass() { return LastLinePass_; }

private:
	//キャトムルロム曲線の描画
	void DrawCatmullRom(const Vector2& controlPoint0, const Vector2& controlPoint1, const Vector2& controlPoint2,
		const Vector2& controlPoint3, const Mat4x4& viewProjectionMatrix);


private:
	//Tex2D
	Texture2D* tex2D_;

	//制御点を表示するためのテクスチャ
	std::vector< std::unique_ptr<Tex2DState>> spheres_;

	//線上を走るテクスチャ
	std::unique_ptr<Tex2DState> runTexture_;

	/*CatmullRom*/
	std::vector<std::unique_ptr<Line>> lines_;

	const Vector2 minusPos_ = { -540.0f,-260.0f };

	Vector2 secondPointPos_ = { 50.0f,100.0f };

	Vector2 thirdPointPos_ = { 100.0f,50.0f };

	Vector2 controlPoint_[4] = {
		{100.0f + minusPos_.x,100.0f + minusPos_.y},
		{400.0f + minusPos_.x,400.0f + minusPos_.y},
		{700.0f + minusPos_.x,100.0f + minusPos_.y},
		{1000.0f + minusPos_.x,200.0f + minusPos_.y}
	};
	//制御点
	std::vector<Vector2> ControlPoints_;
	//線の色
	uint32_t Linecolor_ = 0x000000ff;
	//分割数
	const uint32_t divisionNumber_ = 8;

	float point_ = 0.0f;
	//補完変数
	float t_ = 0.0f;


	//現在移動しているライン
	uint32_t linePass_ = 0;
	//初期ライン
	const uint32_t firstLinePass_ = 0;
	//最終ライン
	uint32_t LastLinePass_ = 2;
	//移動するかどうか
	bool isMove_ = false;
	//描画している数
	uint32_t drawCount_ = 0;
	//追加する数
	int addElementsNum_ = 1;
	//減らす数
	int subtractionElementsNum_ = 1;
	//下のenumに対応させるための変数
	int mode_ = 0;

	enum {
		First,
		Last,
	};

	//曲線一本の長さを調べるための変数群
	std::vector<float> catMullLength_;

	float length_;


	/*ファイル保存関連*/

	using json = nlohmann::json;

	//アイテムのファイルパス
	inline static const std::string kDirectoryPath_ = "Resources/Datas/CatmullRom/";
	//アイテムのファイルパス
	inline static const std::string kDirectoryName_ = "Resources/Datas/CatmullRom";
	//名前
	inline static const std::string kItemName_ = "ControlPoints";

	//このファイルの名前


	//ファイルに保存する
	void SaveFile();
	//ファイルが存在するか確認する
	void ChackFiles();

	bool LoadFiles();

	bool LoadFile(const std::string& groupName);

	bool LoadChackItem(const std::string& directoryPath, const std::string& itemName);

	//imguiの操作をそのまま続けるかどうかのメッセージボックスを表示
	bool OperationConfirmation();

	void from_json(const json& j, Vector2& v);


};

