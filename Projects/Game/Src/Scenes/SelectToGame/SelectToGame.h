#pragma once
#include"Math/Mat4x4.h"

class SelectToGame{
private:
	SelectToGame();
	SelectToGame(const SelectToGame&) = default;
	SelectToGame(SelectToGame&&) = default;
	~SelectToGame() = default;

	SelectToGame& operator=(const SelectToGame&) = default;
	SelectToGame& operator=(SelectToGame&&) = default;

public:
	static constexpr int32_t maxStage_ = 10;

	static SelectToGame* const GetInstance() {
		static SelectToGame instance;

		return &instance;
	};

	/// <summary>
	/// 現在選択しているステージを取得する
	/// </summary>
	/// <returns></returns>
	int32_t GetSelect()const { return selectNum_; }

	/// <summary>
	/// 現在のクリア状況を獲得する
	/// </summary>
	/// <returns></returns>
	std::array<bool, maxStage_>& GetClearFlug() { return isStageClear_; }

	/// <summary>
	/// 選択したステージ
	/// </summary>
	/// <param name="selectNum"></param>
	void SetSelect(const int32_t selectNum) { selectNum_ = selectNum; }

	/// <summary>
	/// クリアフラグのセット
	/// </summary>
	/// <param name="number">クリアしたステージ</param>
	void SetClearFlug(const int32_t number) { isStageClear_[number] = true; }

public:
	/// <summary>
	/// imguiの表示
	/// </summary>
	void Debug();

public:
	int32_t selectNum_;	

	std::array <bool, maxStage_> isStageClear_;

};

