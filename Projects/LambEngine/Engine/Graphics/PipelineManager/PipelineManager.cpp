#include "PipelineManager.h"
#include <cassert>
#include <algorithm>
#include "Engine/Graphics/RootSignature/RootSignature.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Error/Error.h"

Lamb::SafePtr<PipelineManager> PipelineManager::instance_ = nullptr;

void PipelineManager::Initialize() {
	if (instance_) {
		return;
	}
	instance_.reset(new PipelineManager());
	if(instance_){
		Lamb::AddLog("Initialize PipelineManager succeeded");
	}
	else {
		throw Lamb::Error::Code<PipelineManager>("instance is nullptr", __func__);
	}
}
void PipelineManager::Finalize() {
	instance_.reset();
}

void PipelineManager::CreateRootSgnature(D3D12_ROOT_PARAMETER* rootParamater, size_t rootParamaterSize, bool isTexture, bool isOutRangeBorder) {
	auto IsSame = [&rootParamater, &rootParamaterSize, &isTexture,&isOutRangeBorder](const std::unique_ptr<RootSignature>& rootSignature_) {
		return rootSignature_->IsSame(rootParamater, rootParamaterSize, isTexture, isOutRangeBorder);
		};

	auto rootSignatureItr = std::find_if(instance_->rootSignatures_.begin(), instance_->rootSignatures_.end(), IsSame);

	if (rootSignatureItr == instance_->rootSignatures_.end()) {
		auto rootSignature = std::make_unique<RootSignature>();

		rootSignature->Create(rootParamater, rootParamaterSize, isTexture, isOutRangeBorder);

		instance_->rootSignature_ = rootSignature.get();

		instance_->rootSignatures_.push_back(std::move(rootSignature));
	}
	else {
		instance_->rootSignature_ = rootSignatureItr->get();
	}
}

void PipelineManager::SetRootSgnature(RootSignature* rootSignature) {
	instance_->rootSignature_ = rootSignature;
}

void PipelineManager::SetVertexInput(std::string semanticName, uint32_t semanticIndex, DXGI_FORMAT format) {
	instance_->vertexInputStates_.push_back({ semanticName, semanticIndex, format });
}
void PipelineManager::SetShader(const Shader& shader) {
	instance_->shader_ = shader;
}

void PipelineManager::SetState(
	Pipeline::Blend blend,
	Pipeline::SolidState solidState,
	Pipeline::CullMode cullMode,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType,
	uint32_t numRenderTarget
) {
	instance_->blend_ = std::clamp(blend, Pipeline::Blend::None, Pipeline::Blend(Pipeline::Blend::BlendTypeNum - 1));
	instance_->cullMode_ = cullMode;
	instance_->solidState_ = solidState;
	instance_->topologyType_ = topologyType;
	instance_->numRenderTarget_ = numRenderTarget;
}

void PipelineManager::IsDepth(bool isDepth_) {
	instance_->isDepth_ = isDepth_;
}

Pipeline* const PipelineManager::Create() {
	auto IsSmae = [](const std::unique_ptr<Pipeline>& pipeline) {
		bool issame = pipeline->IsSame(
			instance_->shader_,
			instance_->blend_,
			instance_->cullMode_,
			instance_->solidState_,
			instance_->topologyType_,
			instance_->numRenderTarget_,
			instance_->rootSignature_->Get(),
			instance_->isDepth_
		);

		return issame;
		};

	auto pipelineItr = std::find_if(instance_->pipelines_.begin(), instance_->pipelines_.end(), IsSmae);

	if (pipelineItr == instance_->pipelines_.end()) {
		auto pipeline = std::make_unique<Pipeline>();
		pipeline->SetShader(instance_->shader_);
		for (auto& i : instance_->vertexInputStates_) {
			pipeline->SetVertexInput(std::get<0>(i), std::get<1>(i), std::get<2>(i));
		}
		pipeline->Create(
			*instance_->rootSignature_,
			instance_->blend_,
			instance_->cullMode_,
			instance_->solidState_,
			instance_->topologyType_,
			instance_->numRenderTarget_,
			instance_->isDepth_
		);

		if (!pipeline->graphicsPipelineState_) {
			return nullptr;
		}

		instance_->pipelines_.push_back(std::move(pipeline));

		return instance_->pipelines_.rbegin()->get();
	}
	else {
		return pipelineItr->get();
	}
}

void PipelineManager::StateReset() {
	instance_->rootSignature_ = nullptr;
	instance_->shader_ = { nullptr };
	instance_->vertexInputStates_.clear();
	instance_->blend_ = {};
	instance_->cullMode_ = {};
	instance_->solidState_ = {};
	instance_->topologyType_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	instance_->numRenderTarget_ = 0u;
	instance_->isDepth_ = true;
}

PipelineManager::PipelineManager() :
	pipelines_(),
	rootSignatures_(),
	rootSignature_(nullptr),
	shader_{},
	blend_(),
	cullMode_(),
	solidState_(),
	topologyType_(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE),
	numRenderTarget_(0u),
	vertexInputStates_(0),
	isDepth_(true)
{}