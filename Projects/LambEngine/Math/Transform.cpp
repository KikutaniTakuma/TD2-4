#include "Transform.h"
#include <cmath>
#include <imgui.h>
#include "../SoLib/SoLib/SoLib_ImGui.h"

//#include "../../Engine/DirectBase/Create/Create.h"
//#include "../../Engine/DirectBase/Base/DirectXCommon.h"
//
//#include <imgui.h>

Mat4x4 Transform::MakeAffine() const
{
	return Mat4x4::MakeAffin(scale, rotate, translate);
}



void Transform::CalcMatrix()
{
	matWorld_ = MakeAffine();
	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}
}

bool Transform::ImGuiWidget(const std::string &name) {
	if (ImGui::TreeNode((name + "Transform").c_str())) {
		bool isUsing = false;

		isUsing |= ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.001f, 100.f);

		isUsing |= SoLib::ImGuiDragEuler("Rotate", &rotate.x);

		isUsing |= ImGui::DragFloat3("Transform", &translate.x, 0.01f);

		ImGui::TreePop();
		return isUsing;
	}
	return false;
}


bool Transform::ImGuiWidget2D()
{
	if (ImGui::TreeNode("Transform2D")) {
		bool isUsing = false;


		isUsing |= ImGui::DragFloat2("Scale", &scale.x, 0.01f, 0.001f, 2048.f);

		isUsing |= ImGui::DragFloat("Rotate", &rotate.z, Angle::Dig2Rad);

		isUsing |= ImGui::DragFloat2("Transform", &translate.x, 0.01f, -100.f, 100.f);

		ImGui::TreePop();
		return isUsing;
	}
	return false;
}


void Transform::MatToSRT(const Mat4x4 &mat) {

	std::array<__m128, 3u> vec = {
		_mm_loadu_ps(&mat[0][0]),
		_mm_loadu_ps(&mat[1][0]),
		_mm_loadu_ps(&mat[2][0]),
	};

	// スケールの取得
	//scale.x = std::sqrt(mat.m[0][0] * mat.m[0][0] + mat.m[0][1] * mat.m[0][1] + mat.m[0][2] * mat.m[0][2]);
	scale[0u] = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(vec[0u], vec[0u], 0b11111111)));
	scale[1u] = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(vec[1u], vec[1u], 0b11111111)));
	scale[2u] = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(vec[2u], vec[2u], 0b11111111)));

	// 回転行列の取得
	Mat4x4 rotMat;
	_mm_storeu_ps(&rotMat[0][0], _mm_div_ps(vec[0u], _mm_set1_ps(scale.x)));
	_mm_storeu_ps(&rotMat[1][0], _mm_div_ps(vec[1u], _mm_set1_ps(scale.y)));
	_mm_storeu_ps(&rotMat[2][0], _mm_div_ps(vec[2u], _mm_set1_ps(scale.z)));

	// 回転角度の取得
	rotate.x = std::atan2(rotMat[1][2], rotMat[2][2]);
	rotate.y = std::atan2(-rotMat[0][2], std::sqrt(rotMat[1][2] * rotMat[1][2] + rotMat[2][2] * rotMat[2][2]));
	rotate.z = std::atan2(rotMat[0][1], rotMat[0][0]);

	// 移動量の取得
	translate = *reinterpret_cast<const Vector3 *>(&mat[3][0]);
}


void Transform::SetParent(const Transform &parent) {
	parent_ = &parent;
}

void Transform::ConnectParent(const Transform &parent) {
	// 親子関係を解除
	DisConnectParent();
	// 接続する親の逆行列でローカル座標行列を算出
	const Mat4x4 &mat{ matWorld_ * parent.matWorld_.Inverse()};

	// ローカル座標行列からSRTを適用
	this->MatToSRT(mat);
	// 親を設定
	this->SetParent(parent);
	// 座標更新
	this->CalcMatrix();
}

void Transform::DisConnectParent() {
	this->CalcMatrix();
	if (parent_ == nullptr) { return; }

	this->MatToSRT(matWorld_);
	parent_ = nullptr;

	// グローバル座標は変わらないので算出しない
	this->CalcMatrix();
}
