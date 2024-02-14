#include "Model.h"
#include <cassert>
#include <climits>
#include "Utils/ConvertString/ConvertString.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "imgui.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#undef max
#undef min

#include "Error/Error.h"

Shader Model::shader_ = {};

Pipeline* Model::defaultPipeline_ = {};
bool Model::loadShaderFlg_ = false;
bool Model::createGPFlg_ = false;

void Model::Initialize(
	const std::string& vertex,
	const std::string& pixel,
	const std::string& geometory,
	const std::string& hull,
	const std::string& domain
) {
	LoadShader(vertex, pixel, geometory, hull, domain);
	CreateGraphicsPipeline();
}

void Model::LoadShader(
	const std::string& vertex,
	const std::string& pixel,
	const std::string& geometory,
	const std::string& hull,
	const std::string& domain
) {
	static ShaderManager* const shaderManager = ShaderManager::GetInstance();
	if (!loadShaderFlg_) {
		shader_.vertex = shaderManager->LoadVertexShader(vertex);
		shader_.pixel = shaderManager->LoadPixelShader(pixel);
		if (geometory.size() != 0LLU) {
			shader_.geometory = shaderManager->LoadGeometoryShader(geometory);
		}
		if (hull.size() != 0LLU && geometory.size() != 0LLU) {
			shader_.hull = shaderManager->LoadHullShader(hull);
			shader_.domain = shaderManager->LoadHullShader(domain);
		}
		loadShaderFlg_ = true;
	}
}

void Model::CreateGraphicsPipeline() {
	if (loadShaderFlg_) {
		std::array<D3D12_DESCRIPTOR_RANGE, 1> range = {};
		range[0].NumDescriptors = 1;
		range[0].BaseShaderRegister = 0;
		range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		std::array<D3D12_DESCRIPTOR_RANGE, 1> rangeCBV = {};
		rangeCBV[0].NumDescriptors = 4;
		rangeCBV[0].BaseShaderRegister = 0;
		rangeCBV[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		rangeCBV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

		std::array<D3D12_ROOT_PARAMETER, 2> paramates = {};
		paramates[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[0].DescriptorTable.pDescriptorRanges = range.data();
		paramates[0].DescriptorTable.NumDescriptorRanges = UINT(range.size());

		paramates[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[1].DescriptorTable.pDescriptorRanges = rangeCBV.data();
		paramates[1].DescriptorTable.NumDescriptorRanges = UINT(rangeCBV.size());

		PipelineManager::CreateRootSgnature(paramates.data(), paramates.size(), true);

		PipelineManager::SetShader(shader_);

		PipelineManager::SetVertexInput("POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT);
		PipelineManager::SetVertexInput("NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT);
		PipelineManager::SetVertexInput("TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT);

		PipelineManager::SetState(Pipeline::Blend::Normal, Pipeline::SolidState::Solid);

		defaultPipeline_ = PipelineManager::Create();

		PipelineManager::StateReset();

		createGPFlg_ = true;
	}
}

Model::Model() :
	pos(),
	rotate(),
	scale(Vector3::kIdentity),
	color(std::numeric_limits<uint32_t>::max()),
	parent_(nullptr),
	mesh_(nullptr),
	data_(),
	isLoadObj_(false),
	wvpData_(),
	dirLig_(),
	colorBuf_(),
	pipeline_(nullptr)
{

	wvpData_.shaderRegister_ = 0;
	wvpData_->worldMat = Mat4x4::kIdentity;
	wvpData_->viewProjectoionMat = Mat4x4::kIdentity;


	dirLig_.shaderRegister_ = 1;
	light.ligDirection = { 1.0f,-1.0f,-1.0f };
	light.ligDirection = light.ligDirection.Normalize();
	light.ligColor = UintToVector4(0xffffadff).GetVector3();

	light.ptPos = Vector3::kZero;
	light.ptColor = Vector3::kZero;
	light.ptRange = std::numeric_limits<float>::max();

	*dirLig_ = light;

	colorBuf_.shaderRegister_ = 2;
	*colorBuf_ = UintToVector4(color);

	*isLighting_ = 1u;

	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	descriptorHeap->BookingHeapPos(4u);
	descriptorHeap->CreateView(wvpData_);
	descriptorHeap->CreateView(dirLig_);
	descriptorHeap->CreateView(colorBuf_);
	descriptorHeap->CreateView(isLighting_);

}

Model::Model(const std::string& fileName) :
	Model()
{
	this->LoadObj(fileName);
}

Model::Model(const Model& right) :
	Model()
{
	*this = right;
}
Model::Model(Model&& right) noexcept :
	Model()
{
	*this = std::move(right);
}

Model& Model::operator=(const Model& right) {
	pos = right.pos;
	rotate = right.rotate;
	scale = right.scale;
	color = right.color;
	parent_ = right.parent_;

	// 自身がロード済みだったらResourceを解放する
	if (isLoadObj_) {
		data_.clear();
	}

	// rightがロード済みだったら
	if (right.isLoadObj_) {
		mesh_ = right.mesh_;

		if (mesh_) {
			data_ = mesh_->CopyBuffer();
		}
		isLoadObj_ = !!mesh_;
	}

	light = right.light;

	// 定数バッファの値をコピー
	*wvpData_ = *right.wvpData_;
	*dirLig_ = *right.dirLig_;
	*colorBuf_ = *right.colorBuf_;

	pipeline_ = right.pipeline_;

	return *this;
}

Model& Model::operator=(Model&& right) noexcept {
	pos = std::move(right.pos);
	rotate = std::move(right.rotate);
	scale = std::move(right.scale);
	color = std::move(right.color);
	parent_ = std::move(right.parent_);

	// 自身がロード済みだったらResourceを解放する
	if (isLoadObj_) {
		data_.clear();
	}

	// rightがロード済みだったら
	if (right.isLoadObj_) {
		mesh_ = right.mesh_;

		if (mesh_) {
			data_ = mesh_->CopyBuffer();
		}
		isLoadObj_ = !!mesh_;
	}

	light = std::move(right.light);

	// 定数バッファの値をコピー
	*wvpData_ = *right.wvpData_;
	*dirLig_ = *right.dirLig_;
	*colorBuf_ = *right.colorBuf_;


	pipeline_ = std::move(right.pipeline_);

	return *this;
}

void Model::LoadObj(const std::string& fileName) {
	if (!isLoadObj_) {
		mesh_ = MeshManager::GetInstance()->LoadObj(fileName);

		if (!mesh_) {
			throw Lamb::Error::Code<Model>("mesh is nullptr", __func__);
		}

		isLoadObj_ = true;
	}
}

void Model::ThreadLoadObj(const std::string& fileName) {
	if (!isLoadObj_) {
		MeshManager::GetInstance()->LoadObj(fileName, &mesh_);
	}
}

void Model::ChangeTexture(const std::string& useMtlName, const std::string& texName) {
	assert(mesh_);
	if (data_.empty()) {
		data_ = mesh_->CopyBuffer();
	}
	data_[useMtlName].tex = TextureManager::GetInstance()->LoadTexture(texName);
	assert(data_[useMtlName].tex->GetFileName() == texName);
}

void Model::ChangeTexture(const std::string& useMtlName, Texture* tex) {
	assert(tex != nullptr);
	assert(mesh_);
	if (data_.empty()) {
		data_ = mesh_->CopyBuffer();
	}
	data_[useMtlName].tex = tex;
}

void Model::MeshChangeTexture(const std::string& useMtlName, const std::string& texName) {
	mesh_->ChangeTexture(useMtlName, texName);
}

void Model::MeshChangeTexture(const std::string& useMtlName, Texture* tex) {
	assert(tex != nullptr);
	mesh_->ChangeTexture(useMtlName, tex);
}

void Model::SetPipeline(Pipeline* const pipeline)
{
	pipeline_ = pipeline;
}

void Model::Update() {
	*dirLig_ = light;
	wvpData_->worldMat.Affin(scale, rotate, pos);
	if (parent_) {
		wvpData_->worldMat *= parent_->wvpData_->worldMat;
	}

	if (!isLoadObj_ && mesh_ && mesh_->GetIsLoad()) {
		isLoadObj_ = true;
	}
}

void Model::Draw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos) {
	assert(createGPFlg_);
	if (isLoadObj_) {
		if (data_.empty()) {
			data_ = mesh_->CopyBuffer();
		}

		wvpData_->viewProjectoionMat = viewProjectionMat;

		*colorBuf_ = UintToVector4(color);

		light.eyePos = cameraPos;
		dirLig_->eyePos = cameraPos;

		auto commandlist = DirectXCommand::GetInstance()->GetCommandList();

		if (!defaultPipeline_) {
			throw Lamb::Error::Code<Model>("pipeline is nullptr", __func__);
		}

		for (auto& i : data_) {
			if (pipeline_) {
				pipeline_->Use();
			}
			else {
				defaultPipeline_->Use();
			}
			i.second.tex->Use(0);

			commandlist->SetGraphicsRootDescriptorTable(1, wvpData_.GetHandleGPU());

			commandlist->IASetVertexBuffers(0, 1, &i.second.view);
			commandlist->DrawInstanced(i.second.vertNum, 1, 0, 0);
		}
	}
}

void Model::InstancingDraw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos) {
	if (isLoadObj_) {
		light.eyePos = cameraPos;

		mesh_->Use(
			Mat4x4::MakeAffin(scale, rotate, pos),
			viewProjectionMat,
			light,
			UintToVector4(color)
			);
	}
}

void Model::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());

	ImGui::ColorEdit4("モデル本体の色", &colorBuf_->color.r);
	if (ImGui::TreeNode("モデルSRT")) {
		ImGui::DragFloat3("pos", &pos.x, 0.01f);
		ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
		ImGui::DragFloat3("scale", &scale.x, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("ライト")) {
		ImGui::DragFloat3("ligDirection", &light.ligDirection.x, 0.01f);
		light.ligDirection = light.ligDirection.Normalize();
		ImGui::DragFloat3("ligColor", &light.ligColor.x, 0.01f);
		ImGui::DragFloat3("ptPos", &light.ptPos.x, 0.01f);
		ImGui::DragFloat3("ptColor", &light.ptColor.x, 0.01f);
		ImGui::DragFloat("ptRange", &light.ptRange);
		ImGui::TreePop();
	}


	if (ImGui::TreeNode("LoadObj")) {
		auto filePathes = Lamb::GetFilePathFormDir("./Resources/", ".obj");

		for (auto& path : filePathes) {
			if (ImGui::Button(path.string().c_str())) {
				isLoadObj_ = false;
				mesh_ = nullptr;
				data_.clear();
				ThreadLoadObj(path.string());
				break;
			}
		}

		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG
}

Model::~Model() {
	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	descriptorHeap->ReleaseView(wvpData_.GetHandleUINT());
	descriptorHeap->ReleaseView(dirLig_.GetHandleUINT());
	descriptorHeap->ReleaseView(colorBuf_.GetHandleUINT());
	descriptorHeap->ReleaseView(isLighting_.GetHandleUINT());
}