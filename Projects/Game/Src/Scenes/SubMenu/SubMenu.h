#pragma once
#include "Utils/Flg/Flg.h"
#include "./Camera/Camera.h"
#include <memory>

class SubMenu {
public:
	SubMenu();
	~SubMenu() = default;

	SubMenu(const SubMenu&) = delete;
	SubMenu(SubMenu&&) = delete;
	SubMenu& operator=(const SubMenu&) = delete;
	SubMenu& operator=(SubMenu&&) = delete;

public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	void ActiveUpdate();
	virtual void Draw() = 0;

protected:
	virtual void Update() = 0;

public:
	Lamb::Flg isActive_;

protected:
	std::unique_ptr<Camera> camera_;
};