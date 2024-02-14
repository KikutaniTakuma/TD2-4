#pragma once
#include <string>
#include "Math/Vector2.h"

class Framework {
public:
	struct InitDesc {
		std::string windowName = "LambEngine_demo";
		Vector2 windowSize = {1280.0f, 720.0f};
		float maxFps = 60.0f;
		bool isFullesceen = false;
	};

public:
	Framework() = default;
	Framework(const Framework&) = delete;
	Framework(Framework&&) = delete;
	virtual ~Framework() = default;

	Framework& operator=(const Framework&) = delete;
	Framework& operator=(Framework&&) = delete;

protected:
	virtual void Initialize();

	virtual void Finalize();

	virtual void Update() = 0;

	virtual void Draw() = 0;

public:
	void Execution();

protected:
	bool isEnd_;

	InitDesc initDesc_;
};