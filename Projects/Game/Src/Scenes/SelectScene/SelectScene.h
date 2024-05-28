#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/CollisionManager/Collider/Collider.h"
#include "Utils/Easeing/Easeing.h"
#include "Game/SkyDome/SkyDome.h"
#include "Game/CollisionManager/Sphere/Sphere.h"

class SelectScene : public BaseScene {
public:
	SelectScene();
	SelectScene(const SelectScene&) = delete;
	SelectScene(SelectScene&&) = delete;
	~SelectScene() = default;

	SelectScene& operator=(const SelectScene&) = delete;
	SelectScene& operator=(SelectScene&&) = delete;

	struct InGameData{
		float timeLimit;//制限時間
		float clearItemNum; //クリアするための必要個数
	};

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void Debug();

	void SelectMove();

private:
	//Jsonファイルから必要な情報を読み込む
	void LoadGameData(const uint32_t stageNumber);

	void CalcUVPos(const float InGameData, std::array<std::unique_ptr<Tex2DState>, 3>& uvPos);

private:
	//アイテムのファイルパス
	inline static const std::string kDirectoryPath_ = "Resources/Datas/";
	//アイテムのファイルパス
	inline static const std::string kDirectoryName_ = "Resources/Datas";
	//名前
	inline static const std::string kItemName_ = "Stage";

	//必要な情報を描画するためのテクスチャ
	std::array<std::unique_ptr<Tex2DState>, 3> potNumberTexture_;
	//0から1の位、10の位、100の位
	std::array<int32_t, 3> texUVPos_;

	std::array<std::unique_ptr<Tex2DState>, 3> timerNumberTexture_;

	
private:
	//ステージセレクト用の変数
	int32_t selectNum_;
	int32_t oldSelectNum_ = 0;
	static constexpr int32_t kMaxStage_ = 8;

	std::array <bool, kMaxStage_> isStageClear_;

	const float stageInterbal = 800.0f;

	Texture2D* tex2D_;
	std::array<std::unique_ptr<Tex2DState>, kMaxStage_> texies_;
	std::array<std::unique_ptr<Tex2DState>, kMaxStage_> itemTexies_;

	std::array<InGameData, kMaxStage_> inGameDatas_;

	std::array<float, kMaxStage_> startPos_;
	std::array<float, kMaxStage_> endPos_;

	std::array<float, kMaxStage_> startItemPos_;
	std::array<float, kMaxStage_> endItemPos_;
	Vector2 itemDistanceCenter_ = { 300.0f,-140.0f };

	std::array<float, kMaxStage_> easeT_;
	const float kAddEase_ = 0.2f;

	std::unique_ptr<Tex2DState> selectTex_;

	Vector2 shakePower_;

	Easeing ease_;
	
	const uint32_t kCoolTime_ = 20;

	uint32_t coolTime_;

	Audio* selectBGM_ = nullptr;

	Audio* gameDecision_ = nullptr;
	Audio* selectMove_ = nullptr;
	Audio* cancel_ = nullptr;
};

