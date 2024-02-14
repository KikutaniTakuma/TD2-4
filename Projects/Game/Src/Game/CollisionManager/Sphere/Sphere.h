#pragma once

#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include <vector>
#include <memory>
#include <string>

class Sphere {
public:
	Sphere();
	~Sphere();


public:
	void Update();
	void Draw(const Mat4x4& viewProjectionMatrix, uint32_t color);

	void Debug(const std::string guiName);

public:
	float radius;
	Vector3 translation;
	Vector3 worldRoate;

private:
	Vector3 loaclVec;

	Vector3 scale;
	Vector3 rotate;

	Mat4x4 worldMat;

	const int kDivision;

	std::unique_ptr<std::vector<std::vector<Vector3>>> spherePosList;

};