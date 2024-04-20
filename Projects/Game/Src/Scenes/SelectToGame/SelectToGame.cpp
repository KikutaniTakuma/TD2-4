#include "SelectToGame.h"
#include"imgui.h"
SelectToGame::SelectToGame() {
	selectNum_ = 0;
	isStageClear_ = {};
}

void SelectToGame::Debug(){
	ImGui::Begin("セレクトとゲームシーンをまたぐ情報");
	ImGui::Text("選択しているステージ = %d ", selectNum_);
	
	ImGui::End();
}
