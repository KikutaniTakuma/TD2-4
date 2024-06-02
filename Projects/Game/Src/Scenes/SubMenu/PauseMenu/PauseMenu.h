#pragma once
#include "../SubMenu.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/SafePtr/SafePtr.h"

class PauseMenu : public SubMenu {
private:
	enum class State {
		kBack,
		kRetry,
		kStatgeSelect
	};

public:
	PauseMenu() = default;
	~PauseMenu() = default;

	PauseMenu(const PauseMenu&) = delete;
	PauseMenu(PauseMenu&&) = delete;
	PauseMenu& operator=(const PauseMenu&) = delete;
	PauseMenu& operator=(PauseMenu&&) = delete;

public:
	void Initialize() override;
	void Finalize() override;

	void Draw() override;

private:
	void Update() override;

	void ChangeUP();
	void ChangeDown();

	void ChangeState();

	void SceneChange();

	void StateUpdate();

private:
	Lamb::SafePtr<Texture2D> tex2D_;
	std::unique_ptr<Tex2DState> usuGurai_;
	std::unique_ptr<Tex2DState> puaseMenuUI_;
	std::unique_ptr<Tex2DState> menu_;
	std::unique_ptr<Tex2DState> modoruUI_;
	std::unique_ptr<Tex2DState> retryUI_;
	std::unique_ptr<Tex2DState> stageSelectUI_;

	State curretnState_;

	bool isStick_;

	class Audio* modoruSE_ = nullptr;
	class Audio* selectSE_ = nullptr;
	class Audio* choiceSE_ = nullptr;
};