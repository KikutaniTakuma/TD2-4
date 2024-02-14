#include "CloudPipeline.h"
#include <cassert>
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Utils/Random/Random.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

#include "Utils/EngineInfo/EngineInfo.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

#include <fstream>
#include <filesystem>

#include "Engine/Core/WindowFactory/WindowFactory.h"

void CloudPipeline::Update() {
#ifdef _DEBUG
	ImGui::Begin("Cloud");
	ImGui::DragFloat("雲の細やかさ", &(cloudData_->density), 0.01f);
	ImGui::DragFloat("雲の多さ", &(cloudData_->exponent), 0.01f);
	ImGui::DragFloat("雲の色の強さ", &(cloudData_->colorTime), 0.01f);
	ImGui::ColorEdit4("空の色", skyColor_->m.data());
	ImGui::DragFloat2("雲の移動速度", &speed_.x, 0.001f);
	ImGui::Text("randomVec : %f, %f", randomVec_->x, randomVec_->y);
	if (ImGui::Button("保存")) {
		if (!std::filesystem::exists("./Resources/Datas/Cloud/")) {
			std::filesystem::create_directories("./Resources/Datas/Cloud/");
		}

		std::ofstream file{ "./Resources/Datas/Cloud/CloudData.txt" };

		if (file.is_open()) {
			file << cloudData_->density << std::endl
				<< cloudData_->exponent << std::endl
				<< cloudData_->colorTime << std::endl
				<< Vector4ToUint(*skyColor_) << std::endl
				<< speed_.x << std::endl
				<< speed_.y << std::endl;
			file.close();

			MessageBoxA(
				WindowFactory::GetInstance()->GetHwnd(),
				"save succeeded", "Cloud",
				MB_OK | MB_ICONINFORMATION
			);
		}
		else {
			throw Lamb::Error::Code<decltype(*this)>("something Error", __func__);
		}
	}

	ImGui::End();
#endif // _DEBUG


	randomVec_->x += speed_.x * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);
	randomVec_->y += speed_.y * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);

	*colorBuf_ = color;

	wvpMat_->worldMat = worldMat;
	wvpMat_->viewProjection = viewProjection;
}

void CloudPipeline::Use(Pipeline::Blend blendType, bool isDepth) {
	if (isDepth) {
		pipelines_[blendType]->Use();
	}
	else {
		pipelinesNoDepth_[blendType]->Use();
	}
	auto* const commandList = DirectXCommand::GetInstance()->GetCommandList();

	render_->UseThisRenderTargetShaderResource();
	commandList->SetGraphicsRootDescriptorTable(1, wvpMat_.GetHandleGPU());
}

void CloudPipeline::Init(
	const std::string& vsShader,
	const std::string& psShader,
	const std::string& gsFileName,
	const std::string& hsFileName,
	const std::string& dsFileName
) {
	if (width_ == 0u) {
		width_ = static_cast<uint32_t>(Lamb::ClientSize().x);
	}
	if (height_ == 0u) {
		height_ = static_cast<uint32_t>(Lamb::ClientSize().y);
	}

	render_.reset(new RenderTarget{ width_, height_ });

	this->LoadShader(
		vsShader,
		psShader,
		gsFileName,
		hsFileName,
		dsFileName
	);

	std::array<D3D12_DESCRIPTOR_RANGE, 1> renderRange = {};
	renderRange[0].BaseShaderRegister = 0;
	renderRange[0].NumDescriptors = 1;
	renderRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	renderRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> cbvRange = {};
	cbvRange[0].BaseShaderRegister = 0;
	cbvRange[0].NumDescriptors = 5;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	std::array<D3D12_ROOT_PARAMETER, 2> roootParamater = {};
	roootParamater[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	roootParamater[0].DescriptorTable.pDescriptorRanges = renderRange.data();
	roootParamater[0].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(renderRange.size());

	roootParamater[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	roootParamater[1].DescriptorTable.pDescriptorRanges = cbvRange.data();
	roootParamater[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(cbvRange.size());

	PipelineManager::CreateRootSgnature(roootParamater.data(), roootParamater.size(), true);

	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	PipelineManager::SetShader(shader_);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid);

	for (int32_t i = Pipeline::Blend::None; i < Pipeline::Blend::BlendTypeNum; i++) {
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		PipelineManager::IsDepth(true);
		pipelines_[Pipeline::Blend(i)] = PipelineManager::Create();

		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		PipelineManager::IsDepth(false);
		pipelinesNoDepth_[Pipeline::Blend(i)] = PipelineManager::Create();
	}

	PipelineManager::StateReset();


	CbvSrvUavHeap* const srvHeap = CbvSrvUavHeap::GetInstance();

	srvHeap->BookingHeapPos(6u);
	srvHeap->CreateView(*render_);
	srvHeap->CreateView(wvpMat_);
	srvHeap->CreateView(colorBuf_);
	srvHeap->CreateView(randomVec_);
	srvHeap->CreateView(cloudData_);
	srvHeap->CreateView(skyColor_);

	randomVec_->x = Lamb::Random(0.0f, 1.0f);
	randomVec_->y = Lamb::Random(0.0f, 1.0f);

	speed_ = { 0.02f, 0.0f };
	cloudData_->density = 40.18f;
	cloudData_->exponent = 8.57f;
	cloudData_->colorTime = 45.0f;

	*skyColor_ = 0x5FE7FFFF;



	if (std::filesystem::exists("./Resources/Datas/Cloud/CloudData.txt")) {
		std::ifstream file{ "./Resources/Datas/Cloud/CloudData.txt" };

		if (file.is_open()) {
			std::string lineBuf;
			std::getline(file, lineBuf);
			cloudData_->density = static_cast<float>(std::atof(lineBuf.c_str()));
			std::getline(file, lineBuf);
			cloudData_->exponent = static_cast<float>(std::atof(lineBuf.c_str()));
			std::getline(file, lineBuf);
			cloudData_->colorTime = static_cast<float>(std::atof(lineBuf.c_str()));

			std::getline(file, lineBuf);
			*skyColor_ = static_cast<uint32_t>(std::atoll(lineBuf.c_str()));

			std::getline(file, lineBuf);
			speed_.x = static_cast<float>(std::atof(lineBuf.c_str()));
			std::getline(file, lineBuf);
			speed_.y = static_cast<float>(std::atof(lineBuf.c_str()));
		}
	}

}

CloudPipeline::~CloudPipeline() {
	if (render_) {
		auto* const srvHeap = CbvSrvUavHeap::GetInstance();
		srvHeap->ReleaseView(render_->GetHandleUINT());
		srvHeap->ReleaseView(wvpMat_.GetHandleUINT());
		srvHeap->ReleaseView(colorBuf_.GetHandleUINT());
		srvHeap->ReleaseView(randomVec_.GetHandleUINT());
		srvHeap->ReleaseView(cloudData_.GetHandleUINT());
		srvHeap->ReleaseView(skyColor_.GetHandleUINT());
	}

	render_.reset();
}