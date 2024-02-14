#include "Mesh.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "../../TextureManager/TextureManager.h"
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "../MeshManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Error/Error.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include <deque>
#include <algorithm>
#include <filesystem>

Mesh::Mesh() :
	meshs_{},
	texs_{},
	isLoad_(false),
	isObjLoad_(false),
	wvpMats_{256u},
	dirLig_{},
	color_{},
	drawCount_{0u}
{
	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	descriptorHeap->BookingHeapPos(3u);
	descriptorHeap->CreateView(wvpMats_);
	descriptorHeap->CreateView(dirLig_);
	descriptorHeap->CreateView(color_);
}

Mesh::~Mesh() {
	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	descriptorHeap->ReleaseView(wvpMats_.GetHandleUINT());
	descriptorHeap->ReleaseView(dirLig_.GetHandleUINT());
	descriptorHeap->ReleaseView(color_.GetHandleUINT());
}

void Mesh::LoadObj(const std::string& objFileName) {
	if (!isLoad_) {
		std::ifstream file{ objFileName };
		if (file.fail()) {
			if (!std::filesystem::exists(objFileName)) {
				throw Lamb::Error::Code<Mesh>("this file is not exist -> " + objFileName, __func__);
			}
			else {
				throw Lamb::Error::Code<Mesh>("something error -> " + objFileName, __func__);
			}
		}
		

		objFileName_ = objFileName;

		std::vector<Vector4> posDatas(0);

		std::vector<Vector3> normalDatas(0);

		std::vector<Vector2> uvDatas(0);

		std::unordered_map<std::string, std::vector<IndexData>> indexDatas(0);
		std::unordered_map<std::string, std::vector<IndexData>>::iterator indicesItr;

		std::string lineBuf;

		while (std::getline(file, lineBuf)) {
			std::string identifier;
			std::istringstream line(lineBuf);
			line >> identifier;
			if (identifier == "v") {
				Vector4 buf;
				line >> buf.vec.x >> buf.vec.y >> buf.vec.z;
				buf.vec.x *= -1.0f;
				buf.vec.w = 1.0f;

				posDatas.push_back(buf);
			}
			else if (identifier == "vn") {
				Vector3 buf;
				line >> buf.x >> buf.y >> buf.z;
				buf.x *= -1.0f;
				normalDatas.push_back(buf);
			}
			else if (identifier == "vt") {
				Vector2 buf;
				line >> buf.x >> buf.y;
				buf.y = 1.0f - buf.y;
				uvDatas.push_back(buf);
			}
			else if (identifier == "f") {
				std::string buf;
				std::vector<float> posBuf(0);
				std::array<IndexData, 3> indcoes;
				auto idnexItr = indcoes.rbegin();
				while (std::getline(line, buf, ' '))
				{
					/// 0:vertexNumber 1:textureCoordinate 2:NormalNumber
					std::string num[3];
					int32_t count = 0;
					if (std::any_of(buf.cbegin(), buf.cend(), isdigit)) {
						for (auto& ch : buf) {
							if (ch == '/') {
								count++;
							}
							else { num[count] += ch; }
						}
					}

					// エラーチェック
					if (idnexItr == indcoes.rend()) {
						throw Lamb::Error::Code<Mesh>("Not supported for rectangles or more", __func__);
					}

					if (count == 2) {
						idnexItr->vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
						idnexItr->uvNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
						idnexItr->normalNum = static_cast<uint32_t>(std::stoi(num[2]) - 1);
						idnexItr++;
					}
					else if (count == 1) {
						idnexItr->vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
						idnexItr->normalNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
						idnexItr++;
					}
				}
				for (auto& i : indcoes) {
					indicesItr->second.push_back(i);
				}
			}
			else if (identifier == "usemtl") {
				std::string useMtlName;
				line >> useMtlName;
				indexDatas.insert({ useMtlName,std::vector<IndexData>(0) });
				indicesItr = indexDatas.find(useMtlName);
				meshs_[useMtlName];
			}
			else if (identifier == "mtllib") {
				std::string mtlFileName;
				std::filesystem::path path = objFileName;

				line >> mtlFileName;

				LoadMtl(path.parent_path().string() + "/" + mtlFileName);
			}
		}
		file.close();

		for (auto i : indexDatas) {
			// 使用するリソースのサイズは頂点数分のサイズ
			meshs_[i.first].sizeInBytes = static_cast<UINT>(sizeof(VertData) * indexDatas[i.first].size());
			// 1頂点当たりのサイズ
			meshs_[i.first].strideInBytes = sizeof(VertData);

			for (int32_t j = 0; j < indexDatas[i.first].size(); j++) {
				meshs_[i.first].vertices[j].position = posDatas[indexDatas[i.first][j].vertNum];
				meshs_[i.first].vertices[j].normal = normalDatas[indexDatas[i.first][j].normalNum];
				if (!uvDatas.empty()) {
					meshs_[i.first].vertices[j].uv = uvDatas[indexDatas[i.first][j].uvNum];
				}
			}


			meshs_[i.first].vertNum = static_cast<uint32_t>(indexDatas[i.first].size());
		}

		CreateResource();
		meshs_.clear();
		isLoad_ = true;
		isObjLoad_ = true;
	}
}

void Mesh::LoadMtl(const std::string& fileName) {
	std::ifstream file{fileName};
	if (file.fail()) {
		if (!std::filesystem::exists(fileName)) {
			throw Lamb::Error::Code<Mesh>("this file is not exist -> " + fileName, __func__);
		}
		else {
			throw Lamb::Error::Code<Mesh>("something error -> " + fileName, __func__);
		}
	}

	std::string lineBuf;
	std::unordered_map<std::string, Texture*>::iterator texItr;

	std::string useMtlName;
	while (std::getline(file, lineBuf)) {
		std::string identifier;
		std::istringstream line(lineBuf);

		line >> identifier;
		if (identifier == "map_Kd") {
			std::string texName;
			std::filesystem::path path = fileName;

			line >> texName;

			texItr->second = TextureManager::GetInstance()->LoadTexture(path.parent_path().string() + "/" + texName);
		}
		else if (identifier == "newmtl") {
			line >> useMtlName;
			texs_[useMtlName];
			texItr = texs_.find(useMtlName);
		}
	}

	for (auto& i : texs_) {
		if (i.second == nullptr || !(*i.second)) {
			i.second = TextureManager::GetInstance()->GetWhiteTex();
		}
	}
}

void Mesh::ThreadLoadObj(const std::string& objFileName) {
	if (!isLoad_) {
		std::ifstream file{objFileName};
		if (file.fail()) {
			if (!std::filesystem::exists(objFileName)) {
				throw Lamb::Error::Code<Mesh>("this file is not exist -> " + objFileName, __func__);
			}
			else {
				throw Lamb::Error::Code<Mesh>("something error -> " + objFileName, __func__);
			}
		}

		objFileName_ = objFileName;

		std::vector<Vector4> posDatas(0);

		std::vector<Vector3> normalDatas(0);

		std::vector<Vector2> uvDatas(0);

		std::unordered_map<std::string, std::vector<IndexData>> indexDatas(0);
		std::unordered_map<std::string, std::vector<IndexData>>::iterator indicesItr;

		std::string lineBuf;

		while (std::getline(file, lineBuf)) {
			if (Lamb::IsEngineFianlize()) {
				return;
			}

			std::string identifier;
			std::istringstream line(lineBuf);
			line >> identifier;
			if (identifier == "v") {
				Vector4 buf;
				line >> buf.vec.x >> buf.vec.y >> buf.vec.z;
				buf.vec.x *= -1.0f;
				buf.vec.w = 1.0f;

				posDatas.push_back(buf);
			}
			else if (identifier == "vn") {
				Vector3 buf;
				line >> buf.x >> buf.y >> buf.z;
				buf.x *= -1.0f;
				normalDatas.push_back(buf);
			}
			else if (identifier == "vt") {
				Vector2 buf;
				line >> buf.x >> buf.y;
				buf.y = 1.0f - buf.y;
				uvDatas.push_back(buf);
			}
			else if (identifier == "f") {
				std::string buf;
				std::vector<float> posBuf(0);
				std::array<IndexData, 3> indcoes;
				auto idnexItr = indcoes.rbegin();
				while (std::getline(line, buf, ' '))
				{
					/// 0:vertexNumber 1:textureCoordinate 2:NormalNumber
					std::string num[3];
					int32_t count = 0;
					if (std::any_of(buf.cbegin(), buf.cend(), isdigit)) {
						for (auto& ch : buf) {
							if (ch == '/') {
								count++;
							}
							else { num[count] += ch; }
						}
					}

					// エラーチェック
					if (idnexItr == indcoes.rend()) {
						throw Lamb::Error::Code<Mesh>("Not supported for rectangles or more", "ThreadLoadObj()");
					}

					if (count == 2) {
						idnexItr->vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
						idnexItr->uvNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
						idnexItr->normalNum = static_cast<uint32_t>(std::stoi(num[2]) - 1);
						idnexItr++;
					}
					else if (count == 1) {
						idnexItr->vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
						idnexItr->normalNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
						idnexItr++;
					}
				}
				for (auto& i : indcoes) {
					indicesItr->second.push_back(i);
				}
			}
			else if (identifier == "usemtl") {
				std::string useMtlName;
				line >> useMtlName;
				indexDatas.insert({ useMtlName,std::vector<IndexData>(0) });
				indicesItr = indexDatas.find(useMtlName);
				meshs_[useMtlName];
			}
			else if (identifier == "mtllib") {
				std::string mtlFileName;
				std::filesystem::path path = objFileName;

				line >> mtlFileName;

				ThreadLoadMtl(path.parent_path().string() + "/" + mtlFileName);
			}
		}
		file.close();

		for (auto i : indexDatas) {
			// 使用するリソースのサイズは頂点数分のサイズ
			meshs_[i.first].sizeInBytes = static_cast<UINT>(sizeof(VertData) * indexDatas[i.first].size());
			// 1頂点当たりのサイズ
			meshs_[i.first].strideInBytes = sizeof(VertData);

			for (int32_t j = 0; j < indexDatas[i.first].size(); j++) {
				meshs_[i.first].vertices[j].position = posDatas[indexDatas[i.first][j].vertNum];
				meshs_[i.first].vertices[j].normal = normalDatas[indexDatas[i.first][j].normalNum];
				if (!uvDatas.empty()) {
					meshs_[i.first].vertices[j].uv = uvDatas[indexDatas[i.first][j].uvNum];
				}
			}


			meshs_[i.first].vertNum = static_cast<uint32_t>(indexDatas[i.first].size());
		}

		isObjLoad_ = true;
	}
}
void Mesh::ThreadLoadMtl(const std::string& fileName) {
	std::ifstream file{fileName};
	if (file.fail()) {
		if (!std::filesystem::exists(fileName)) {
			throw Lamb::Error::Code<Mesh>("this file is not exist -> " + fileName, __func__);
		}
		else {
			throw Lamb::Error::Code<Mesh>("something error -> " + fileName, __func__);
		}
	}
	std::string lineBuf;
	std::unordered_map<std::string, Texture*>::iterator texItr;
	std::unordered_map<std::string, bool> isTexLoad;
	std::unordered_map<std::string, bool>::iterator isTexLoadItr;

	std::string useMtlName;
	while (std::getline(file, lineBuf)) {
		std::string identifier;
		std::istringstream line(lineBuf);

		line >> identifier;
		if (identifier == "map_Kd") {
			std::string texName;
			std::filesystem::path path = fileName;

			line >> texName;

			TextureManager::GetInstance()->LoadTexture(path.parent_path().string() + "/" + texName, &texItr->second);
			isTexLoadItr->second = true;
		}
		else if (identifier == "newmtl") {
			line >> useMtlName;
			texs_[useMtlName];
			texItr = texs_.find(useMtlName);

			isTexLoad.insert({ useMtlName, false });
			isTexLoadItr = isTexLoad.find(useMtlName);
		}
	}

	for (auto& i : texs_) {
		if (!isTexLoad[i.first]) {
			i.second = TextureManager::GetInstance()->GetWhiteTex();
		}
	}
}

void Mesh::CheckModelTextureLoadFinish() {
	if (!isLoad_ && isObjLoad_) {
		for (auto& i : texs_) {
			// nullptrかまだロードが完了していない
			if (!i.second || !i.second->CanUse()) {
				isLoad_ = false;
				return;
			}
		}

		CreateResource();
		meshs_.clear();
		isLoad_ = true;
	}
}

void Mesh::CreateResource() {
	ReleaseResource();

	for (auto& mesh : meshs_) {
		// コンテナに追加
		resource_[mesh.first];
		// resource生成
		resource_[mesh.first].resource.first = DirectXDevice::GetInstance()->CreateBufferResuorce(mesh.second.sizeInBytes);
		resource_[mesh.first].resource.first.SetName<Mesh>();
		// view情報追加
		resource_[mesh.first].resource.second.BufferLocation = resource_[mesh.first].resource.first->GetGPUVirtualAddress();
		resource_[mesh.first].resource.second.SizeInBytes = mesh.second.sizeInBytes;
		resource_[mesh.first].resource.second.StrideInBytes = mesh.second.strideInBytes;
		
		// 頂点情報コピー
		VertData* vertMap = nullptr;
		resource_[mesh.first].resource.first->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));

		for (auto& vert : mesh.second.vertices) {
			assert(vertMap);
			vertMap[vert.first] = vert.second;
		}

		resource_[mesh.first].resource.first->Unmap(0, nullptr);

		// 頂点数追加
		resource_[mesh.first].vertNum = mesh.second.vertNum;

		// テクスチャ追加
		resource_[mesh.first].tex = texs_[mesh.first];
	}
}

void Mesh::ReleaseResource() {
	for (auto& i : resource_) {
		if (i.second.resource.first) {
			i.second.resource.first->Release();
			i.second.resource.first = nullptr;
		}
	}
}

std::unordered_map<std::string, Mesh::CopyData> Mesh::CopyBuffer() const {
	std::unordered_map<std::string, Mesh::CopyData> copyData;
	for (auto& i : resource_) {
		copyData[i.first];
		copyData[i.first] = i.second;
	}
	return copyData;
}

void Mesh::ResetDrawCount() {
	drawCount_ = 0u;
}

void Mesh::ResetBufferSize() {
	wvpMats_.Resize(1u);
}

void Mesh::ResizeBuffers() {
	if (wvpMats_.Size() <= drawCount_) {
		uint32_t nextBufferSize = wvpMats_.Size() * 2u;
		wvpMats_.Resize(nextBufferSize);
	}
}

void Mesh::Use(
	const Mat4x4& wvpMat,
	const Mat4x4& viewOrojection,
	const Mesh::Light& light,
	const Vector4& color
) {
	ResizeBuffers();

	wvpMats_[drawCount_].worldMat = wvpMat;
	wvpMats_[drawCount_].viewProjectoionMat = viewOrojection;
	*dirLig_ = light;
	*color_ = color;

	drawCount_++;
}

void Mesh::Draw() {
	if (0u < drawCount_) {
		auto commandList = DirectXCommand::GetInstance()->GetCommandList();

		if (!pipeline_) {
			throw Lamb::Error::Code<Mesh>("pipeline is nullptr", __func__);
		}

		for (auto& i : resource_) {
			pipeline_->Use();
			i.second.tex->Use(0);

			commandList->SetGraphicsRootDescriptorTable(1, wvpMats_.GetHandleGPU());
			commandList->SetGraphicsRootDescriptorTable(2, dirLig_.GetHandleGPU());

			commandList->IASetVertexBuffers(0, 1, &i.second.resource.second);
			commandList->DrawInstanced(i.second.vertNum, drawCount_, 0, 0);
		}
	}
}

void Mesh::ChangeTexture(const std::string& useMtlName, const std::string& texName) {
	resource_[useMtlName].tex = TextureManager::GetInstance()->LoadTexture(texName);
	assert(resource_[useMtlName].tex->GetFileName() == texName);
}

void Mesh::ChangeTexture(const std::string& useMtlName, Texture* tex) {
	assert(tex != nullptr);
	resource_[useMtlName].tex = tex;
}

Shader Mesh::shader_ = {};

Pipeline* Mesh::pipeline_ = {};
bool Mesh::loadShaderFlg_ = false;
bool Mesh::createGPFlg_ = false;

void Mesh::Initialize(
	const std::string& vertex,
	const std::string& pixel,
	const std::string& geometory,
	const std::string& hull,
	const std::string& domain
) {
	LoadShader(vertex, pixel, geometory, hull, domain);
	CreateGraphicsPipeline();
}

void Mesh::LoadShader(
	const std::string& vertex,
	const std::string& pixel,
	const std::string& geometory,
	const std::string& hull,
	const std::string& domain
) {
	static ShaderManager* const shaderManager = shaderManager->GetInstance();
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

void Mesh::CreateGraphicsPipeline() {
	if (loadShaderFlg_) {
		std::array<D3D12_DESCRIPTOR_RANGE, 1> rangeTex = {};
		rangeTex[0].NumDescriptors = 1;
		rangeTex[0].BaseShaderRegister = 0;
		rangeTex[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		rangeTex[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		std::array<D3D12_DESCRIPTOR_RANGE, 1> rangeWvp = {};
		rangeWvp[0].NumDescriptors = 1;
		rangeWvp[0].BaseShaderRegister = 1;
		rangeWvp[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		rangeWvp[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		std::array<D3D12_DESCRIPTOR_RANGE, 1> rangeCBV = {};
		rangeCBV[0].NumDescriptors = 2;
		rangeCBV[0].BaseShaderRegister = 0;
		rangeCBV[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		rangeCBV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;


		std::array<D3D12_ROOT_PARAMETER, 3> paramates = {};
		paramates[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[0].DescriptorTable.pDescriptorRanges = rangeTex.data();
		paramates[0].DescriptorTable.NumDescriptorRanges = UINT(rangeTex.size());

		paramates[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[1].DescriptorTable.pDescriptorRanges = rangeWvp.data();
		paramates[1].DescriptorTable.NumDescriptorRanges = UINT(rangeWvp.size());

		paramates[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[2].DescriptorTable.pDescriptorRanges = rangeCBV.data();
		paramates[2].DescriptorTable.NumDescriptorRanges = UINT(rangeCBV.size());

		PipelineManager::CreateRootSgnature(paramates.data(), paramates.size(), true);

		PipelineManager::SetShader(shader_);

		PipelineManager::SetVertexInput("POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT);
		PipelineManager::SetVertexInput("NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT);
		PipelineManager::SetVertexInput("TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT);

		PipelineManager::IsDepth();

		PipelineManager::SetState(Pipeline::Blend::None, Pipeline::SolidState::Solid);

		pipeline_ = PipelineManager::Create();

		PipelineManager::StateReset();

		createGPFlg_ = true;
	}
}