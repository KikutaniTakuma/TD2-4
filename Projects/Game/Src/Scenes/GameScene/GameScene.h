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

#include "SoLib/IO/File.h"
#include "SoLib/IO/CSV.h"
#include "SoLib/Containers/Array2D.h"

#include "GameObject/GameObject.h"
#include "Game/GameManager/GameManager.h"

#include "Game/CollisionManager/AABB/AABB.h"

#include "Game/CollisionManager/CollisionManager.h"

#include "Editor/BlockEditor/BlockEditor.h"
#include "Editor/EnemyEditor/EnemyEditor.h"
#include <Game/GameUIManager/GameUIManager.h>

#include "../SubMenu/PauseMenu/PauseMenu.h"

class GameScene : public BaseScene {
public:
	GameScene();
	GameScene(const GameScene&) = delete;
	GameScene(GameScene&&) = delete;
	~GameScene() = default;

	GameScene& operator=(const GameScene&) = delete;
	GameScene& operator=(GameScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void ChangeToResult();

private:
	void TextureInitialize();

	void TextureUpdate();

	void CloudReset(const uint32_t cloudNumber);

	void TextureDraw();

	void Debug();


private:

	void CalcUVPos(const float InGameData, std::array<std::unique_ptr<Tex2DState>, 3>& uvPos);

private:

	//class Water *water_ = nullptr;

	

	GameManager* gameManager_ = nullptr;

	std::unique_ptr<GameUIManager> gameUIManager_ = nullptr;

	Vector2 shakePower_;

	// std::unique_ptr<Player> player_;

	CollisionManager* collisionManager_ = nullptr;

	/*std::unique_ptr<SkyDome> skydome_ = nullptr;
	class Cloud *cloud_ = nullptr;*/

	//bool editorMode_ = false;

	//bool enemyMode_ = false;

	const float editorCameraPosZ_ = -42.0f;

	//std::unique_ptr<BlockEditor> blockEditor_;

	//std::unique_ptr<EnemyEditor> enemyEditor_;

	//EnemyManager* enemyManager_ = nullptr;

	Texture2D* tex2D_ = nullptr;

	Audio* gameBGM_ = nullptr;

	Audio* cancel_ = nullptr;

	Audio* telop_ = nullptr;

	Audio* gameSet_ = nullptr;

	static const uint32_t kCloudNum_ = 8;

	//雲
	std::array<std::unique_ptr<Tex2DState>, kCloudNum_> clouds_;

	std::array<float, kCloudNum_> cloudsSpeed_;

	std::unordered_map<std::string, Vector2> cloudScale_;

	std::array<std::string, 3> cloudType_;
	//操作UI
	//std::unique_ptr<Tex2DState> spaceTex_, keyTex_;

	std::unique_ptr<Tex2DState> backGround_;

	//目的表示のフレーム
	std::unique_ptr<Tex2DState> objectiveFrame_;
	//目的表示の背景の暗いやつ
	std::unique_ptr<Tex2DState> objectiveBackGround_;

	//演出が終わったかどうか
	Lamb::Flg isEndObjective_ = false;

	bool isFirstLoadFlag_ = true;

	///////////*時計とかのための*///////////
	//必要な情報を描画するためのテクスチャ
	std::array<std::unique_ptr<Tex2DState>, 3> potNumberTexture_;
	//0から1の位、10の位、100の位
	std::array<int32_t, 3> texUVPos_;

	std::array<std::unique_ptr<Tex2DState>, 3> timerNumberTexture_;

	//時計本体
	std::unique_ptr<Tex2DState> timerState_;
	//時計の針
	std::unique_ptr<Tex2DState> clockHandsState_;
	//釜
	std::unique_ptr<Tex2DState> potState_;
	//クリア必要個数
	Vector3 potNumberPos_;
	//制限時間
	Vector3 clockNumberPos_;

	Vector3 timerPos_;

	Vector3 clockHandsPos_;

	Vector3 potPos_;

	Easeing ease_;

	bool isFadeOut_ = false;

	//始点と終点
	Vector2 easePoint_ = { 900.0f,0.0f };

	int32_t easeCount_ = 90;


	float numberDistance_;

	std::unique_ptr<SubMenu> pause_;

	bool isFirstUpdate_ = false;

	bool isFirstSound_ = true;
};