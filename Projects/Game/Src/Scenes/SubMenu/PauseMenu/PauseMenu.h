#pragma once
#include "../SubMenu.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/SafePtr/SafePtr.h"

class PauseMenu : public SubMenu {
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

private:
	Lamb::SafePtr<Texture2D> tex2D_;
	std::unique_ptr<Tex2DState> usuGurai_;
	std::unique_ptr<Tex2DState> puaseMenuUI_;
	std::unique_ptr<Tex2DState> menu_;
	std::unique_ptr<Tex2DState> modoru_;
	std::unique_ptr<Tex2DState> modoruUI_;
	std::unique_ptr<Tex2DState> retry_;
	std::unique_ptr<Tex2DState> stageSelect_;

	uint32_t gamepadUITextureID_;

	bool isRetryChoice_;
	bool isStick_;

	class Audio* modoruSE_;
	class Audio* selectSE_;
	class Audio* choiceSE_;
};