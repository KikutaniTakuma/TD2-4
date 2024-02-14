#include "Sphere.h"
#include "Drawers/Line/Line.h"
#include <numbers>
#include <cassert>
#include "imgui.h"

Sphere::Sphere() :
	radius(1.0f),
	loaclVec(radius,0.0f,0.0f),
	scale(1.0f,1.0f,1.0f),
	rotate(0.0f,0.0f,0.0f),
	translation(0.0f,0.0f,10.0f),
	worldMat(),
	kDivision(10),
	spherePosList(std::make_unique<std::vector<std::vector<Vector3>>>(0))
{
	for (int y = 0; y <= kDivision; y++) {
		rotate.z = -std::numbers::pi_v<float> / 2.0f;
		spherePosList->push_back(std::vector<Vector3>(0));
		for (int z = 0; z <= kDivision; z++) {
			worldMat = Mat4x4::MakeRotateZ(rotate.z) * Mat4x4::MakeRotateY(rotate.y) * Mat4x4::MakeAffin(scale, worldRoate, translation);
			spherePosList->rbegin()->push_back(loaclVec * worldMat);
			rotate.z += (std::numbers::pi_v<float> / static_cast<float>(kDivision));
		}

		rotate.y += (std::numbers::pi_v<float> / static_cast<float>(kDivision)) * 2.0f;
	}
	
}

Sphere::~Sphere() {
	spherePosList.reset();
}


void Sphere::Update() {
	spherePosList->clear();
	rotate = Vector3::kZero;

	loaclVec.x = radius;

	for (int y = 0; y <= kDivision; y++) {
		rotate.z = -std::numbers::pi_v<float> / 2.0f;
		spherePosList->push_back(std::vector<Vector3>(0));
		for (int z = 0; z <= kDivision; z++) {
			worldMat = Mat4x4::MakeRotateZ(rotate.z) * Mat4x4::MakeRotateY(rotate.y) * Mat4x4::MakeAffin(scale, worldRoate, translation);
			spherePosList->rbegin()->push_back(loaclVec * worldMat);
			rotate.z += (std::numbers::pi_v<float> / static_cast<float>(kDivision));
		}

		rotate.y += (std::numbers::pi_v<float> / static_cast<float>(kDivision)) * 2.0f;
	}
}

void Sphere::Draw(const Mat4x4& viewProjectionMatrix, uint32_t color) {
	std::vector<Vector3> preHarfCircle;
	std::vector<Vector3>::iterator preItr;
	for (auto i = spherePosList->begin(); i != spherePosList->end(); ++i) {
		if (i != spherePosList->begin()) {
			preItr = preHarfCircle.begin();
		}

		for (auto j = i->begin(); j != i->end();) {
			Vector3& startPos = *j;
			if (++j != i->rbegin().base()) {
				Vector3& endPos = *j;
				Line::Draw(startPos, endPos, viewProjectionMatrix, color);

				if (i != spherePosList->begin()) {
					preItr++;
					Vector3& prePos = (*preItr);
					Line::Draw(prePos, endPos, viewProjectionMatrix, color);
				}
			}
		}
		preHarfCircle = (*i);
	}
}

void Sphere::Debug([[maybe_unused]] const std::string guiName)
{
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat("radius", &radius, 0.01f);
	ImGui::DragFloat3("rotate", &worldRoate.x, 0.01f);
	ImGui::DragFloat3("pos", &translation.x, 0.01f);
	ImGui::End();
}