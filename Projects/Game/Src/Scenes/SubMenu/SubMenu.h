#pragma once
#include "Utils/Flg/Flg.h"
#include "./Camera/Camera.h"
#include <memory>

class SubMenu {
public:
	SubMenu();
	virtual ~SubMenu() = default;

	SubMenu(const SubMenu&) = delete;
	SubMenu(SubMenu&&) = delete;
	SubMenu& operator=(const SubMenu&) = delete;
	SubMenu& operator=(SubMenu&&) = delete;

public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	void SetSceneManger(class SceneManager* sceneManager) {
		sceneManager_ = sceneManager;
	}

	void ActiveUpdate();

	void ActiveDraw();

protected:
	virtual void Draw() = 0;
	virtual void Update() = 0;

public:
	Lamb::Flg isActive_;

protected:
	std::unique_ptr<Camera> camera_;

	class SceneManager* sceneManager_;
};