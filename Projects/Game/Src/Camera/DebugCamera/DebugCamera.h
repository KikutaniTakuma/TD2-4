#pragma once
#include "Camera/Camera.h"

class DebugCamera final : public Camera {
public:
	DebugCamera();
	DebugCamera(const DebugCamera&) = default;
	DebugCamera(DebugCamera&&) = default;
	~DebugCamera() = default;

public:
	DebugCamera& operator=(const DebugCamera&) = default;
	DebugCamera& operator=(DebugCamera&&) = default;

public:
	bool operator==(const DebugCamera&) const noexcept = default;
	bool operator!=(const DebugCamera&) const noexcept = default;

private:
	void Update() override;

	void Debug(const std::string& guiName) override;

private:
	float scaleSpeed_;
	float rotateSpeed_;
	float translateSpeed_;

	bool isRotateInverse_;
	bool isTranslateInverse_;
};