#pragma once
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Camera/Camera.h"
#include <memory>

class Slime {
public:
	Slime() = default;
	~Slime() = default;
	Slime(const Slime&) = delete;
	Slime(Slime&&) = delete;

	Slime& operator=(const Slime&) = delete;
	Slime& operator=(Slime&&) = delete;

public:
	void Init();

	void Update();

	void Draw(const Camera& camera);

private:
	std::unique_ptr<Texture2D> body_;
	std::unique_ptr<Texture2D> core_;
	std::unique_ptr<PeraRender> pera_;
	std::unique_ptr<Camera> peraCamera_;

	class SlimePipeline* slimePipeline_;
};