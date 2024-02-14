#pragma once
#include "Drawers/Particle/Particle.h"
#include "./Camera/Camera.h"

class ParticleEditor {
private:
	ParticleEditor() = default;
	ParticleEditor(const ParticleEditor&) = delete;
	ParticleEditor(ParticleEditor&&) = delete;
	~ParticleEditor() = default;

	ParticleEditor& operator=(const ParticleEditor&) = delete;
	ParticleEditor& operator=(ParticleEditor&&) = delete;

public:
	static void Initialize();

	static void Finalize();

	static ParticleEditor* GetInstance();

private:
	static ParticleEditor* instance_;

public:
	void Editor();

	void Draw(const Camera& camera);

private:
	std::string inputLoadString_;
	std::string currentLoadString_;
	Particle particle_;

	bool isOpen_ = false;

	Camera camera_;

	bool is3DCamera_ = false;
	bool isLocalCamera_ = true;
};