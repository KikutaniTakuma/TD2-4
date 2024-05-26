#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Particle/Particle.h"
#include "Utils/Easeing/Easeing.h"
#include "Game/FlaskParticle/FlaskParticle.h"
#include "Math/Transform.h"


class ResultScene : public BaseScene{
private:
	enum class EffectState{
		kFirst,
		kSecond,
		kGameOver,
		kGameClear	
	};

	enum class CurrentUIPick {
		kToNext,
		kRetry,
		kToStageSelect,
	};

public:
	ResultScene();
	ResultScene(const ResultScene&) = delete;
	ResultScene(ResultScene&&) = delete;
	~ResultScene() = default;

	ResultScene& operator=(const ResultScene&) = delete;
	ResultScene& operator=(ResultScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void Debug();

	static void SetIsGameClear(bool isGameClear) {
		isGameClear_ = isGameClear;
	}

private:
	bool isSkip_ = false;

	Lamb::SafePtr<Texture2D> tex2D_;
	std::unique_ptr<Tex2DState> backGround_;
	float backUV_;
	//std::unique_ptr<Particle> backGroundParticle_;
	std::unique_ptr<Easeing> backGroundEase_;
	Vector3 backGroundStartPos_;
	Vector3 backGroundEndPos_;
	Lamb::Flg isFirstActive_;


	std::array<std::unique_ptr<FlaskParticle>, 5> flaskParticles_;
	float curretnActiveFlaskParticleNum_;
	float allFlaskParticleNum_;
	Vector2 flaskParticleAppDurationMin;
	Vector2 flaskParticleAppDurationMax;
	std::unique_ptr<Easeing> flaskParticleAppDurationEase_;

	std::unique_ptr<Particle> cauldronParticle_;
	std::unique_ptr<Transform> cauldronTransform_;
	std::unique_ptr<Tex2DAniamtor> cauldronAnimator_;
	uint32_t cauldronTextureID_;
	Vector3 cauldronBasisPos_;
	std::pair<Vector3, Vector3> cauldronShake_;
	std::pair<Vector3, Vector3> cauldronScale_;
	std::unique_ptr<Easeing> cauldronEase_;
	

	uint32_t zanennTexID_;
	//uint32_t clearTextureID_;

	// ゲームクリアならture。falseならゲームオーバー
	static bool isGameClear_;
	// リザルトに入る前にプレイしていたステージ番号
	uint32_t preGameStageNumber_;

	EffectState effectStatus_;


	// UI関係
	// 次へ
	std::unique_ptr<Tex2DState> toNextUI_;
	// タイトルへ
	std::unique_ptr<Tex2DState> retryUI_;
	// ステージセレクトへ
	std::unique_ptr<Tex2DState> toStageSelectUI_;

	// リザルトメッセージ
	std::unique_ptr<Tex2DState> resultMessage_;
	std::unique_ptr<Easeing> resultMessageEase_;
	Vector2 resultMessageYPos_;

	// クリア時のアイテム
	std::unique_ptr<Tex2DState> clearItem_;
	std::unique_ptr<Easeing> clearItemEase_;
	std::unique_ptr<Easeing> clearItemScaleEase_;
	Vector2 clearItemYPos_;
	std::pair<Vector3, Vector3> clearItemScaleDuration_;
	std::pair<Vector3, Vector3> clearItemScaleDurationLoop_;
	std::unique_ptr<Particle> clearItemParticle_;

	bool isStickInput_ = false;

	CurrentUIPick currentUIPick_;

	std::unique_ptr<Tex2DState> witch_;
	std::unique_ptr<Tex2DAniamtor> witchAnimator_;
	std::unique_ptr<Tex2DAniamtor> witchResultAnimator_;
	uint32_t witchCryTexID_ = 0;
	uint32_t witchHappyTexID_ = 0;
	uint32_t witchFlyAwayTexID_ = 0;
	uint32_t witchCraftTexID_ = 0;
	std::unique_ptr<Easeing> witchMoveX_;
	bool isStartWitchMoveX_ = false;
	std::unique_ptr<Easeing> witchMoveY_;
	bool isStartWitchMoveY_ = false;
	std::unique_ptr<Easeing> witchMoveY2_;
	bool isStartWitchMoveY2_ = false;
	bool withcEaseingEnd_ = false;

	bool isFirstEnd_ = false;


	// ああゲームの音～
	Audio* gameDecision_ = nullptr;
	Audio* selectMove_ = nullptr;
	Audio* witchCraft_ = nullptr;
	Audio* witchGameOverBGM_ = nullptr;
	Audio* witchGameClearBGM_ = nullptr;
	Audio* witchCraftExplotion_ = nullptr;

private:
	void FirstEffect();
	void FirstDraw();
	// 最初の演出(大釜に素材が集まる)
	void SecondEffect();
	void SecondDraw();

	void FlyAway();
	void WitchDraw();

	// ゲームクリア時の演出(何かが錬成される)
	void GameClearEffect();
	void GameClearDraw();

	// ゲームオーバー時の演出
	void GameOverEffect();
	void GameOverDraw();

	// 動かない板ポリの描画
	void DrawConstantUI();

	// UIの更新
	void UpdateUI();

	// UIの描画
	void DrawUI();

	void Skip();
};

