#include "SelectToGame.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include"imgui.h"
SelectToGame::SelectToGame() {
	selectNum_ = 0;
	isStageClear_ = {};

	Lamb::SafePtr texManager = TextureManager::GetInstance();
	stageItemTextureIDs[0] = texManager->LoadTexture("./Resources/UI/Item/diamond.png"); // でぇーあもんど
	stageItemTextureIDs[1] = texManager->LoadTexture("./Resources/UI/Item/pendant.png"); // ペンダント
	stageItemTextureIDs[2] = texManager->LoadTexture("./Resources/UI/Item/orb.png"); // 未定
	stageItemTextureIDs[3] = texManager->LoadTexture("./Resources/UI/Item/poisonPotion.png"); // ポーション
	stageItemTextureIDs[4] = texManager->LoadTexture("./Resources/UI/Item/ring.png"); // 指輪
	stageItemTextureIDs[5] = texManager->LoadTexture("./Resources/UI/Item/sord.png"); // オーブ
	stageItemTextureIDs[6] = texManager->LoadTexture("./Resources/UI/Item/elixir.png"); // 剣
	stageItemTextureIDs[7] = texManager->LoadTexture("./Resources/UI/Item/human.png");   // 未定

}

uint32_t SelectToGame::GetStageItemTextureID(int32_t stageNumber) const {
	return stageItemTextureIDs[std::clamp(stageNumber, 0, kMaxStage - 1)];
}

void SelectToGame::Debug(){
#ifdef _DEBUG
	ImGui::Begin("セレクトとゲームシーンをまたぐ情報");
	ImGui::Text("選択しているステージ = %d ", selectNum_);
	
	ImGui::End();
#endif // _DEBUG
}
