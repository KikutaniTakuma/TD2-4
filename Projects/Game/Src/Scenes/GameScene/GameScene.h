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

	void ChangeToResult() { sceneManager_->SceneChange(BaseScene::ID::Result); }

private:
	void TextureInitialize();

	void TextureUpdate();

	void CloudReset(const uint32_t cloudNumber);

	void TextureDraw();

	void Debug();


private:
	//class Water *water_ = nullptr;

	

	GameManager* gameManager_ = nullptr;

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

	//一の位
	std::unique_ptr<Tex2DState> dwarfNumTex_;
	//十の位
	std::unique_ptr<Tex2DState> dwarfTenNumTex_;

	static const uint32_t kCloudNum_ = 8;

	//雲
	std::array<std::unique_ptr<Tex2DState>, kCloudNum_> clouds_;

	std::array<float, kCloudNum_> cloudsSpeed_;

	std::unordered_map<std::string, Vector2> cloudScale_;

	std::array<std::string, 3> cloudType_;
	//操作UI
	//std::unique_ptr<Tex2DState> spaceTex_, keyTex_;

	std::unique_ptr<Tex2DState> backGround_;



};