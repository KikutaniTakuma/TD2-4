#include "RenderContextManager.h"
#include "Utils/SafeDelete/SafeDelete.h"

#include "../Shader/ShaderManager/ShaderManager.h"

#include "../../Core/DescriptorHeap/CbvSrvUavHeap.h"

#include "../PipelineManager/PipelineManager.h"

Lamb::SafePtr<RenderContextManager> RenderContextManager::instance_ = nullptr;

RenderContextManager::~RenderContextManager()
{
}

RenderContextManager* const RenderContextManager::GetInstance() {
	return instance_.get();
}

void RenderContextManager::Initialize() {
	instance_.reset(new RenderContextManager());
}

void RenderContextManager::Finalize() {
	instance_.reset();
}

RenderSet* const RenderContextManager::Get(const LoadFileNames& fileNames)
{
	std::unordered_map<Key, std::unique_ptr<RenderSet>>& currentRenderData = isNowThreading_ ? threadRenderData_ : renderData_;
	auto isExist = currentRenderData.find(fileNames);

	if (isExist == currentRenderData.end()) {
		Load(fileNames);
	}

	return currentRenderData[fileNames].get();
}

void RenderContextManager::SetIsNowThreading(bool isNowThreading) {
	isNowThreading_ = isNowThreading;

	if (not isNowThreading_) {
		for (auto& i : threadRenderData_) {
			renderData_.insert(std::make_pair(i.first, i.second.release()));
		}

		threadRenderData_.clear();
	}
}

void RenderContextManager::Draw() {
	for (auto& i : renderData_) {
		i.second->Draw();
	}
}

void RenderContextManager::ResetDrawCount() {
	for (auto& i : renderData_) {
		i.second->ResetDrawCount();
	}
}

Shader RenderContextManager::LoadShader(const ShaderFileNames& shaderName)
{
	Shader result;

	ShaderManager* const shaderMaanger = ShaderManager::GetInstance();


	result.vertex = shaderMaanger->LoadVertexShader(shaderName.vsFileName);
	result.pixel = shaderMaanger->LoadPixelShader(shaderName.psFileName);
	result.geometory = shaderMaanger->LoadGeometoryShader(shaderName.gsFileName);
	result.hull = shaderMaanger->LoadHullShader(shaderName.hsFileName);
	result.domain = shaderMaanger->LoadDomainShader(shaderName.dsFileName);

	return result;
}

std::array<Pipeline*, BlendType::kNum> RenderContextManager::CreateGraphicsPipelines(Shader shader)
{
	std::array<Pipeline*, BlendType::kNum> result = {nullptr};

	auto srvHeap = CbvSrvUavHeap::GetInstance();


	std::array<D3D12_DESCRIPTOR_RANGE, 1> cbvRange = {};
	cbvRange[0].NumDescriptors = 1;
	cbvRange[0].BaseShaderRegister = 0;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

	uint32_t baseShaderRegister = 0;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> srvRange = {};
	srvRange[0].NumDescriptors = 3;
	srvRange[0].BaseShaderRegister = baseShaderRegister;
	srvRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	srvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	baseShaderRegister = srvRange[0].NumDescriptors;

	std::array<D3D12_DESCRIPTOR_RANGE, 1> texRange = {};
	texRange[0].NumDescriptors = srvHeap->GetMaxTexture();
	texRange[0].BaseShaderRegister = baseShaderRegister;
	texRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;


	std::array<D3D12_ROOT_PARAMETER, 3> rootPrams = {};
	rootPrams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[0].DescriptorTable.NumDescriptorRanges = UINT(cbvRange.size());
	rootPrams[0].DescriptorTable.pDescriptorRanges = cbvRange.data();
	rootPrams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[1].DescriptorTable.NumDescriptorRanges = UINT(srvRange.size());
	rootPrams[1].DescriptorTable.pDescriptorRanges = srvRange.data();
	rootPrams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[2].DescriptorTable.NumDescriptorRanges = UINT(texRange.size());
	rootPrams[2].DescriptorTable.pDescriptorRanges = texRange.data();
	rootPrams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	PipelineManager::CreateRootSgnature(rootPrams.data(), rootPrams.size(), true, false);
	PipelineManager::SetShader(shader);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetVertexInput("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	PipelineManager::SetVertexInput("BLENDINDICES", 0, DXGI_FORMAT_R32_UINT);

	for (size_t i = 0; i < size_t(BlendType::kNum); i++) {
		size_t blendType = i < Pipeline::Blend::BlendTypeNum ? i : i - Pipeline::Blend::BlendTypeNum;

		PipelineManager::IsDepth(i < Pipeline::Blend::BlendTypeNum);
		PipelineManager::SetState(Pipeline::Blend(blendType), Pipeline::SolidState::Solid);
		result[i] = PipelineManager::Create();
	}


	PipelineManager::StateReset();

	return result;
}
