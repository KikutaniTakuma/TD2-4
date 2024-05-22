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
private:
	Lamb::SafePtr<Texture2D> tex2D_;
	std::unique_ptr<Transform> backGround_;
	uint32_t backGroundTextureID_;
	//std::unique_ptr<Particle> backGroundParticle_;
	std::unique_ptr<Easeing> backGroundEase_;
	Vector3 backGroundStartPos_;
	Vector3 backGroundEndPos_;
	Lamb::Flg isFirstActive_;


	std::array<std::unique_ptr<FlaskParticle>, 4> flaskParticles_;
	std::array<uint32_t, 4> flaskTextureID_;
	float curretnActiveFlaskParticleNum_;
	float allFlaskParticleNum_;

	std::unique_ptr<Particle> cauldronParticle_;
	std::unique_ptr<Transform> cauldronTransform_;
	uint32_t cauldronTextureID_;
	Vector3 cauldronBasisPos_;
	std::pair<Vector3, Vector3> cauldronShake_;
	std::pair<Vector3, Vector3> cauldronScale_;
	std::unique_ptr<Easeing> cauldronEase_;
	

	uint32_t zanennTexID_;
	//uint32_t clearTextureID_;

	// ゲームクリアならture。falseならゲームオーバー
	bool isGameClear_;

	EffectState effectStatus_;


	// UI関係
	// 次へ
	std::unique_ptr<Tex2DState> toNextUI_;
	// タイトルへ
	std::unique_ptr<Tex2DState> retryUI_;
	// ステージセレクトへ
	std::unique_ptr<Tex2DState> toStageSelectUI_;


	CurrentUIPick currentUIPick_;

private:
	// 最初の演出(大釜に素材が集まる)
	void FirstEffect();
	void FirstDraw();

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
};

