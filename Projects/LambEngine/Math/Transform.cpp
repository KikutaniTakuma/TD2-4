#include "Transform.h"
#include <cmath>

//#include "../../Engine/DirectBase/Create/Create.h"
//#include "../../Engine/DirectBase/Base/DirectXCommon.h"
//
//#include <imgui.h>

Mat4x4 BaseTransform::MakeAffine() const
{
	return Mat4x4::MakeAffin(scale, rotate, translate);
}



void BaseTransform::CalcMatrix()
{
	matWorld_ = MakeAffine();
	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}
}

bool BaseTransform::ImGuiWidget(const std::string &name) {
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


bool BaseTransform::ImGuiWidget2D()
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


void BaseTransform::MatToSRT(const Mat4x4 &mat) {

	std::array<__m128, 3u> vec = {
		_mm_load_ps(mat[0].data()),
		_mm_load_ps(mat[1].data()),
		_mm_load_ps(mat[2].data()),
	};

	// スケールの取得
	//scale.x = std::sqrt(mat.m[0][0] * mat.m[0][0] + mat.m[0][1] * mat.m[0][1] + mat.m[0][2] * mat.m[0][2]);
	scale.arr[0u] = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(vec[0u], vec[0u], 0b11110001)));
	scale.arr[1u] = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(vec[1u], vec[1u], 0b11110001)));
	scale.arr[2u] = std::sqrt(_mm_cvtss_f32(_mm_dp_ps(vec[2u], vec[2u], 0b11110001)));

	// 回転行列の取得
	Matrix4x4 rotMat = Matrix4x4::Identity();
	if (scale != Vector3::one) {
		*reinterpret_cast<__m128 *>(rotMat.m[0].data()) = _mm_div_ps(vec[0u], _mm_set1_ps(scale.x));
		*reinterpret_cast<__m128 *>(rotMat.m[1].data()) = _mm_div_ps(vec[1u], _mm_set1_ps(scale.y));
		*reinterpret_cast<__m128 *>(rotMat.m[2].data()) = _mm_div_ps(vec[2u], _mm_set1_ps(scale.z));
	}

	// 回転角度の取得
	rotate.x = std::atan2(rotMat.m[1][2], rotMat.m[2][2]);
	rotate.y = std::atan2(-rotMat.m[0][2], std::sqrt(rotMat.m[1][2] * rotMat.m[1][2] + rotMat.m[2][2] * rotMat.m[2][2]));
	rotate.z = std::atan2(rotMat.m[0][1], rotMat.m[0][0]);

	// 移動量の取得
	translate = *reinterpret_cast<const Vector3 *>(mat.m[3].data());
}


void BaseTransform::SetParent(const BaseTransform &parent) {
	parent_ = &parent;
}

void BaseTransform::ConnectParent(const BaseTransform &parent) {
	// 親子関係を解除
	DisConnectParent();
	// 接続する親の逆行列でローカル座標行列を算出
	const Mat4x4 &mat{ matWorld_ * parent.matWorld_.MakeInverse() };

	// ローカル座標行列からSRTを適用
	this->MatToSRT(mat);
	// 親を設定
	this->SetParent(parent);
	// 座標更新
	this->CalcMatrix();
}

void BaseTransform::DisConnectParent() {
	this->CalcMatrix();
	if (parent_ == nullptr) { return; }

	this->MatToSRT(matWorld_);
	parent_ = nullptr;

	// グローバル座標は変わらないので算出しない
	this->CalcMatrix();
}
