#include "Particle.h"
#include "imgui.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"

#include "Utils/UtilsLib/UtilsLib.h"
#include "Utils/Random/Random.h"
#include "Error/Error.h"

#include "../externals/nlohmann/json.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>

/// <summary>
/// 静的変数のインスタンス化
/// </summary>

std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> Particle::graphicsPipelineState_ = {};
Shader Particle::shader_ = {};

D3D12_INDEX_BUFFER_VIEW Particle::indexView_ = {};
Lamb::LambPtr<ID3D12Resource> Particle::indexResource_ = nullptr;

void Particle::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	LoadShader(vsFileName, psFileName);

	uint16_t indices[] = {
			0,1,3, 1,2,3
	};
	indexResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(indices));
	indexResource_.SetName<Particle>();
	indexView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexView_.SizeInBytes = sizeof(indices);
	indexView_.Format = DXGI_FORMAT_R16_UINT;
	uint16_t* indexMap = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	for (int32_t i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}
	indexResource_->Unmap(0, nullptr);

	CreateGraphicsPipeline();
}

void Particle::Finalize() {
	indexResource_.Reset();
}

void Particle::LoadShader(const std::string& vsFileName, const std::string& psFileName) {
	static ShaderManager* const shaderManager = shaderManager->GetInstance();
	shader_.vertex = shaderManager->LoadVertexShader(vsFileName);
	assert(shader_.vertex);
	shader_.pixel = shaderManager->LoadPixelShader(psFileName);
	assert(shader_.pixel);
}

void Particle::CreateGraphicsPipeline() {
	std::array<D3D12_DESCRIPTOR_RANGE,1> texRange = {};
	texRange[0].NumDescriptors = 1;
	texRange[0].BaseShaderRegister = 0;
	texRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	std::array<D3D12_DESCRIPTOR_RANGE, 1> strucBufRange = {};
	strucBufRange[0].NumDescriptors = 2;
	strucBufRange[0].BaseShaderRegister = 1;
	strucBufRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	strucBufRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	std::array<D3D12_ROOT_PARAMETER, 2> rootPrams={};
	rootPrams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[0].DescriptorTable.NumDescriptorRanges = UINT(texRange.size());
	rootPrams[0].DescriptorTable.pDescriptorRanges = texRange.data();
	rootPrams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[1].DescriptorTable.NumDescriptorRanges = UINT(strucBufRange.size());
	rootPrams[1].DescriptorTable.pDescriptorRanges = strucBufRange.data();
	rootPrams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	PipelineManager::CreateRootSgnature(rootPrams.data(), rootPrams.size(), true);
	PipelineManager::SetShader(shader_);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	PipelineManager::IsDepth(false);

	for (int32_t i = Pipeline::Blend::None; i < Pipeline::Blend::BlendTypeNum; i++) {
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		graphicsPipelineState_[i] = PipelineManager::Create();
	}

	PipelineManager::StateReset();

	for (auto& i : graphicsPipelineState_) {
		if (!i) {
			throw Lamb::Error::Code<Particle>("pipeline is nullptr", __func__);
		}
	}
}

Particle::Particle() :
	wtfs_(),
	uvPibot(),
	uvSize(Vector2::kIdentity),
	tex_(TextureManager::GetInstance()->GetWhiteTex()),
	isLoad_(false),
	isBillboard_(true),
	isYBillboard_(false),
	wvpMat_(1),
	colorBuf_(1),
	aniStartTime_(),
	aniCount_(0.0f),
	uvPibotSpd_(0.0f),
	isAnimation_(0.0f),
	settings_(),
	currentSettingIndex_(0u),
	currentParticleIndex_(0u),
	srvHeap_(nullptr),
	isClose_{false}
{
	srvHeap_ = CbvSrvUavHeap::GetInstance();
	srvHeap_->BookingHeapPos(2u);
	srvHeap_->CreateView(wvpMat_);
	srvHeap_->CreateView(colorBuf_);
	for (uint32_t i = 0; i < wvpMat_.Size(); i++) {
		wvpMat_[i] = Mat4x4::kIdentity;
	}

	for (uint32_t i = 0; i < colorBuf_.Size(); i++) {
		colorBuf_[i] = Vector4::kIdentity;
	}

	vertexResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(VertexData) * 4);
	vertexResource_.SetName<Particle>();

	vertexView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexView_.SizeInBytes = sizeof(VertexData) * 4;
	vertexView_.StrideInBytes = sizeof(VertexData);

	wtfs_.resize(1);

	for (size_t i = 0; i < wtfs_.size(); i++) {
		wtfs_[i].scale_ = Vector2::kIdentity * 512.0f;
		wtfs_[i].pos_.x = 10.0f * i;
		wtfs_[i].pos_.y = 10.0f * i;
		wtfs_[i].pos_.z += 0.3f;
	}

	const std::filesystem::path kDirectoryPath = "./Resources/Datas/Particles/";

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}

	std::filesystem::directory_iterator dirItr(kDirectoryPath);

	dataDirectoryName_ = "particle0";

	uint32_t dirCount = 0u;

	// directory内のファイルをすべて読み込む
	for (const auto& entry : dirItr) {
		if (dataDirectoryName_ == entry.path().string()) {
			dataDirectoryName_ = "particle" + std::to_string(dirCount);
		}
		dirCount++;
	}

}

Particle::Particle(uint32_t indexNum) :
	wtfs_(),
	uvPibot(),
	uvSize(Vector2::kIdentity),
	tex_(TextureManager::GetInstance()->GetWhiteTex()),
	isLoad_(false),
	isBillboard_(true),
	isYBillboard_(false),
	wvpMat_(indexNum),
	colorBuf_(indexNum),
	aniStartTime_(),
	aniCount_(0.0f),
	uvPibotSpd_(0.0f),
	isAnimation_(0.0f),
	settings_(),
	currentSettingIndex_(0u),
	currentParticleIndex_(0u),
	srvHeap_(nullptr),
	isClose_{false}
{
	srvHeap_ = CbvSrvUavHeap::GetInstance();
	srvHeap_->BookingHeapPos(2u);
	srvHeap_->CreateView(wvpMat_);
	srvHeap_->CreateView(colorBuf_);

	for (uint32_t i = 0; i < wvpMat_.Size();i++) {
		wvpMat_[i] = Mat4x4::kIdentity;
	}
	
	for (uint32_t i = 0; i < colorBuf_.Size(); i++) {
		colorBuf_[i] = Vector4::kIdentity;
	}

	if (vertexResource_) {
		vertexResource_.Reset();
		vertexResource_ = nullptr;
	}

	vertexResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(VertexData) * 4);
	vertexResource_.SetName<Particle>();

	vertexView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexView_.SizeInBytes = sizeof(VertexData) * 4;
	vertexView_.StrideInBytes = sizeof(VertexData);

	wtfs_.resize(indexNum);

	for (size_t i = 0; i < wtfs_.size();i++) {
		wtfs_[i].scale_ = Vector2::kIdentity * 512.0f;
		wtfs_[i].pos_.x = 10.0f * i;
		wtfs_[i].pos_.y = 10.0f * i;
		wtfs_[i].pos_.z += 0.3f;
	}

	const std::filesystem::path kDirectoryPath = "./Resources/Datas/Particles/";

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}

	std::filesystem::directory_iterator dirItr(kDirectoryPath);

	dataDirectoryName_ = "particle0";

	uint32_t dirCount = 0u;

	// directory内のファイルをすべて読み込む
	for (const auto& entry : dirItr) {
		if (dataDirectoryName_ == entry.path().string()) {
			dataDirectoryName_ = "particle" + std::to_string(dirCount);
		}
		dirCount++;
	}
}

Particle::Particle(const Particle& right) :
	Particle(right.wvpMat_.Size())
{
	*this = right;
}
Particle::Particle(Particle&& right) noexcept :
	Particle(right.wvpMat_.Size())
{
	*this = std::move(right);
}

Particle& Particle::operator=(const Particle& right) {
	assert(wvpMat_.Size() == right.wvpMat_.Size());

	wtfs_ = right.wtfs_;

	uvPibot = right.uvPibot;
	uvSize = right.uvSize;

	tex_ = right.tex_;
	isLoad_ = right.isLoad_;

	for (uint32_t i = 0; i < wvpMat_.Size();i++) {
		wvpMat_[i] = right.wvpMat_[i];
	}
	
	for (uint32_t i = 0; i < colorBuf_.Size(); i++) {
		colorBuf_[i] = right.colorBuf_[i];
	}

	aniStartTime_ = right.aniStartTime_;
	aniCount_ = right.aniCount_;
	isAnimation_ = right.isAnimation_;
	uvPibotSpd_ = right.uvPibotSpd_;

	emitterPos = right.emitterPos;

	datas_ = right.datas_;
	dataDirectoryName_ = right.dataDirectoryName_;

	settings_ = right.settings_;

	isLoop_ = right.isLoop_;

	currentSettingIndex_ = right.currentSettingIndex_;
	currentParticleIndex_ = right.currentParticleIndex_;

	return *this;
}

Particle& Particle::operator=(Particle&& right) noexcept {
	wtfs_ = std::move(right.wtfs_);

	uvPibot = std::move(right.uvPibot);
	uvSize = std::move(right.uvSize);

	tex_ = std::move(right.tex_);
	isLoad_ = std::move(right.isLoad_);

	for (uint32_t i = 0; i < wvpMat_.Size(); i++) {
		wvpMat_[i] = right.wvpMat_[i];
	}
	for (uint32_t i = 0; i < colorBuf_.Size(); i++) {
		colorBuf_[i] = right.colorBuf_[i];
	}

	aniStartTime_ = std::move(right.aniStartTime_);
	aniCount_ = std::move(right.aniCount_);
	isAnimation_ = std::move(right.isAnimation_);
	uvPibotSpd_ = std::move(right.uvPibotSpd_);


	emitterPos = std::move(right.emitterPos);

	datas_ = std::move(right.datas_);
	dataDirectoryName_ = std::move(right.dataDirectoryName_);

	settings_ = std::move(right.settings_);

	isLoop_ = std::move(right.isLoop_);

	currentSettingIndex_ = std::move(right.currentSettingIndex_);
	currentParticleIndex_ = std::move(right.currentParticleIndex_);

	return *this;
}

void Particle::Resize(uint32_t index) {
	wvpMat_.Resize(index);
	colorBuf_.Resize(index);
	wtfs_.resize(index);
}

Particle::~Particle() {
//#ifdef _DEBUG
//
//
//	for (auto i = 0llu; i < settings.size(); i++) {
//		const auto groupName = ("setting" + std::to_string(i));
//
//		datas[groupName]["Emitter_Pos"] = settings[i].emitter.pos;
//		datas[groupName]["Emitter_Size"] = settings[i].emitter.size;
//		datas[groupName]["Emitter_Type"] = static_cast<uint32_t>(settings[i].emitter.type);
//		datas[groupName]["Emitter_CircleSize"] = settings[i].emitter.circleSize;
//		datas[groupName]["Emitter_CircleSize"] = settings[i].emitter.circleSize;
//		datas[groupName]["Emitter_RotateFirst"] = settings[i].emitter.rotate.first;
//		datas[groupName]["Emitter_RotateSecond"] = settings[i].emitter.rotate.second;
//		datas[groupName]["Emitter_ParticleMaxNum"] = settings[i].emitter.particleMaxNum;
//		datas[groupName]["Emitter_vaildTime"] = static_cast<uint32_t>(settings[i].emitter.validTime.count());
//
//		// 大きさ
//		datas[groupName]["Particle_isSameHW"] = static_cast<uint32_t>(settings[i].isSameHW);
//		datas[groupName]["Particle_size1"] = settings[i].size.first;
//		datas[groupName]["Particle_size2"] = settings[i].size.second;
//		datas[groupName]["Particle_sizeSecond1"] = settings[i].sizeSecond.first;
//		datas[groupName]["Particle_sizeSecond2"] = settings[i].sizeSecond.second;
//		datas[groupName]["Particle_sizeEase"] = static_cast<uint32_t>(settings[i].colorEaseType);
//
//		// 速度
//		datas[groupName]["Particle_velocity1"] = settings[i].velocity.first;
//		datas[groupName]["Particle_velocity2"] = settings[i].velocity.second;
//		datas[groupName]["Particle_velocitySecond1"] = settings[i].velocitySecond.first;
//		datas[groupName]["Particle_velocitySecond2"] = settings[i].velocitySecond.second;
//		datas[groupName]["Particle_ease"] = static_cast<uint32_t>(settings[i].moveEaseType);
//		datas[groupName]["Particle_moveRotateFirst"] = settings[i].moveRotate.first;
//		datas[groupName]["Particle_moveRotateSecond"] = settings[i].moveRotate.second;
//		
//		// 回転
//		datas[groupName]["Particle_rotate1"] = settings[i].rotate.first;
//		datas[groupName]["Particle_rotate2"] = settings[i].rotate.second;
//		datas[groupName]["Particle_rotateSecond1"] = settings[i].rotateSecond.first;
//		datas[groupName]["Particle_rotateSecond2"] = settings[i].rotateSecond.second;
//		datas[groupName]["Particle_rotateEase"] = static_cast<uint32_t>(settings[i].rotateEaseType);
//
//		datas[groupName]["Particle_particleNumFirst"] = settings[i].particleNum.first;
//		datas[groupName]["Particle_particleNumSecond"] = settings[i].particleNum.second;
//		datas[groupName]["Particle_freqFirst"] = settings[i].freq.first;
//		datas[groupName]["Particle_freqSecond"] = settings[i].freq.second;
//		datas[groupName]["Particle_deathFirst"] = settings[i].death.first;
//		datas[groupName]["Particle_deathSecond"] = settings[i].death.second;
//		datas[groupName]["Particle_colorFirst"] = settings[i].color.first;
//		datas[groupName]["Particle_colorSecond"] = settings[i].color.second;
//		datas[groupName]["Particle_colorEase"] = static_cast<uint32_t>(settings[i].colorEaseType);
//		BackUpSettingFile("setting" + std::to_string(i));
//	}
//
//	std::ofstream file{ dataDirectoryName + "BackUp/" + "delete_otherSetting.txt" };
//
//	if (!file.fail() && isLoad) {
//		file << static_cast<bool>(isLoop_) << std::endl
//			<< tex->GetFileName();
//	}
//#endif // _DEBUG

	srvHeap_->ReleaseView(wvpMat_.GetHandleUINT());
	srvHeap_->ReleaseView(colorBuf_.GetHandleUINT());
}


void Particle::LoadTexture(const std::string& fileName) {
	static TextureManager* textureManager = TextureManager::GetInstance();
	assert(textureManager);
	while (textureManager->IsNowThreadLoading()) {
		// 非同期読み込みが終わるまでビジーループ
	}

	tex_ = textureManager->LoadTexture(fileName);

	if (tex_ && !isLoad_) {
		isLoad_ = true;
	}
}

void Particle::ThreadLoadTexture(const std::string& fileName) {
	tex_ = nullptr;
	TextureManager::GetInstance()->LoadTexture(fileName, &tex_);
	isLoad_ = false;
}


void Particle::LoadSettingDirectory(const std::string& directoryName) {
	settings_.clear();
	datas_.clear();

	const std::filesystem::path kDirectoryPath = "./Resources/Datas/Particles/" + directoryName + "/";
	dataDirectoryName_ = kDirectoryPath.string();

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}

	std::filesystem::directory_iterator dirItr(kDirectoryPath);
	
	// directory内のファイルをすべて読み込む
	for (const auto& entry : dirItr) {
		const auto& filePath = entry.path();


		// jsonファイルじゃなかったら飛ばす
		if (filePath.extension() != L".json") {
			continue;
		}

		// jsonファイルを読み込む
		LopadSettingFile(filePath.string());
	}

	std::ifstream file{ dataDirectoryName_ + "otherSetting.txt" };

	if (!file.fail()) {
		std::string lineBuf;
		if (std::getline(file, lineBuf)) {
			isLoop_ = static_cast<bool>(std::atoi(lineBuf.c_str()));
		}
		else {
			isLoop_ = false;
		}

		if (std::getline(file, lineBuf)) {
			this->LoadTexture(lineBuf);
		}
		else {
			tex_ = TextureManager::GetInstance()->GetWhiteTex();
			isLoad_ = true;
		}
		if (std::getline(file, lineBuf)) {
			isBillboard_ = static_cast<bool>(std::atoi(lineBuf.c_str()));
		}
		else {
			isBillboard_ = true;
		}
		if (std::getline(file, lineBuf)) {
			isYBillboard_ = static_cast<bool>(std::atoi(lineBuf.c_str()));
		}
		else {
			isYBillboard_ = false;
		}


		file.close();
	}

	isClose_ = false;
}

void Particle::LopadSettingFile(const std::string& jsonName) {
	std::ifstream file(jsonName);

	if (file.fail()) {
		return;
	}

	nlohmann::json root;

	file >> root;

	file.close();

	std::filesystem::path groupName = jsonName;
	groupName = groupName.stem();

	nlohmann::json::iterator groupItr = root.find(groupName);

	assert(groupItr != root.end());

	for (auto itemItr = groupItr->begin(); itemItr != groupItr->end(); itemItr++) {
		const std::string& itemName = itemItr.key();

		if (itemItr->is_number_integer()) {
			uint32_t value = itemItr->get<uint32_t>();
			auto& gruop = datas_[groupName.string()];
			Item item = value;
			gruop[itemName] = value;
		}
		else if (itemItr->is_number_float()) {
			float value = itemItr->get<float>();
			auto& gruop = datas_[groupName.string()];
			Item item = value;
			gruop[itemName] = value;
		}
		else if (itemItr->is_array() && itemItr->size() == 2) {
			Vector2 value = { itemItr->at(0),itemItr->at(1) };
			auto& gruop = datas_[groupName.string()];
			Item item = value;
			gruop[itemName] = value;
		}
		else if (itemItr->is_array() && itemItr->size() == 3) {
			Vector3 value = { itemItr->at(0),itemItr->at(1),itemItr->at(2) };
			auto& gruop = datas_[groupName.string()];
			Item item = value;
			gruop[itemName] = value;
		}
		else if (itemItr->is_string()) {
			std::string value = itemItr->get<std::string>();
			auto& gruop = datas_[groupName.string()];
			Item item = value;
			gruop[itemName] = value;
		}
	}

	settings_.push_back(Setting{});
	auto& setting = settings_.back();

	setting.emitter_.pos_ = std::get<Vector3>(datas_[groupName.string()]["Emitter_Pos"]);
	setting.emitter_.size_ = std::get<Vector3>(datas_[groupName.string()]["Emitter_Size"]);
	setting.emitter_.type_ = static_cast<EmitterType>(std::get<uint32_t>(datas_[groupName.string()]["Emitter_Type"]));
	setting.emitter_.circleSize_ = std::get<float>(datas_[groupName.string()]["Emitter_CircleSize"]);
	setting.emitter_.rotate_.first = std::get<Vector3>(datas_[groupName.string()]["Emitter_RotateFirst"]);
	setting.emitter_.rotate_.second = std::get<Vector3>(datas_[groupName.string()]["Emitter_RotateSecond"]);
	setting.emitter_.particleMaxNum_ = std::get<uint32_t>(datas_[groupName.string()]["Emitter_ParticleMaxNum"]);
	setting.emitter_.validTime_ = std::chrono::milliseconds(std::get<uint32_t>(datas_[groupName.string()]["Emitter_vaildTime"]));

	setting.isSameHW_ = static_cast<bool>(std::get<uint32_t>(datas_[groupName.string()]["Particle_isSameHW"]));
	setting.size_.first = std::get<Vector2>(datas_[groupName.string()]["Particle_size1"]);
	setting.size_.second = std::get<Vector2>(datas_[groupName.string()]["Particle_size2"]);
	setting.sizeSecond_.first = std::get<Vector2>(datas_[groupName.string()]["Particle_sizeSecond1"]);
	setting.sizeSecond_.second = std::get<Vector2>(datas_[groupName.string()]["Particle_sizeSecond2"]);
	setting.sizeEaseType_ = static_cast<int32_t>(std::get<uint32_t>(datas_[groupName.string()]["Particle_sizeEase"]));
	setting.sizeEase_ = Easing::GetFunction(setting.sizeEaseType_);

	setting.velocity_.first = std::get<Vector3>(datas_[groupName.string()]["Particle_velocity1"]);
	setting.velocity_.second = std::get<Vector3>(datas_[groupName.string()]["Particle_velocity2"]);
	setting.velocitySecond_.first =std::get<Vector3>(datas_[groupName.string()]["Particle_velocitySecond1"]);
	setting.velocitySecond_.second  =std::get<Vector3>(datas_[groupName.string()]["Particle_velocitySecond2"]);
	setting.moveRotate_.first = std::get<Vector3>(datas_[groupName.string()]["Particle_moveRotateFirst"]);
	setting.moveRotate_.second = std::get<Vector3>(datas_[groupName.string()]["Particle_moveRotateSecond"]);
	
	setting.rotate_.first = std::get<Vector3>(datas_[groupName.string()]["Particle_rotate1"]);
	setting.rotate_.second = std::get<Vector3>(datas_[groupName.string()]["Particle_rotate2"]);
	setting.rotateSecond_.first = std::get<Vector3>(datas_[groupName.string()]["Particle_rotateSecond1"]);
	setting.rotateSecond_.second = std::get<Vector3>(datas_[groupName.string()]["Particle_rotateSecond2"]);
	setting.rotateEaseType_ = static_cast<int32_t>(std::get<uint32_t>(datas_[groupName.string()]["Particle_rotateEase"]));
	setting.rotateEase_ = Easing::GetFunction(setting.rotateEaseType_);

	setting.particleNum_.first = std::get<uint32_t>(datas_[groupName.string()]["Particle_particleNumFirst"]);
	setting.particleNum_.second = std::get<uint32_t>(datas_[groupName.string()]["Particle_particleNumSecond"]);
	setting.freq_.first = std::get<uint32_t>(datas_[groupName.string()]["Particle_freqFirst"]);
	setting.freq_.second = std::get<uint32_t>(datas_[groupName.string()]["Particle_freqSecond"]);
	setting.death_.first = std::get<uint32_t>(datas_[groupName.string()]["Particle_deathFirst"]);
	setting.death_.second = std::get<uint32_t>(datas_[groupName.string()]["Particle_deathSecond"]);
	setting.color_.first = std::get<uint32_t>(datas_[groupName.string()]["Particle_colorFirst"]);
	setting.color_.second = std::get<uint32_t>(datas_[groupName.string()]["Particle_colorSecond"]);
	setting.colorEaseType_ = static_cast<int32_t>(std::get<uint32_t>(datas_[groupName.string()]["Particle_colorEase"]));
	setting.colorEase_ = Easing::GetFunction(setting.colorEaseType_);

	setting.moveEaseType = static_cast<int32_t>(std::get<uint32_t>(datas_[groupName.string()]["Particle_ease"]));
	setting.moveEase_ = Easing::GetFunction(setting.moveEaseType);
}

void Particle::SaveSettingFile(const std::string& groupName) {
	auto itrGroup = datas_.find(groupName);
	assert(itrGroup != datas_.end());

	nlohmann::json root;

	root = nlohmann::json::object();

	root[groupName] = nlohmann::json::object();

	for (auto itemItr = itrGroup->second.begin(); itemItr != itrGroup->second.end(); itemItr++) {
		const std::string& itemName = itemItr->first;

		auto& item = itemItr->second;

		if (std::holds_alternative<uint32_t>(item)) {
			root[groupName][itemName] = std::get<uint32_t>(item);
		}
		else if (std::holds_alternative<float>(item)) {
			root[groupName][itemName] = std::get<float>(item);
		}
		else if (std::holds_alternative<Vector2>(item)) {
			auto tmp = std::get<Vector2>(item);
			root[groupName][itemName] = nlohmann::json::array({ tmp.x, tmp.y });
		}
		else if (std::holds_alternative<Vector3>(item)) {
			auto tmp = std::get<Vector3>(item);
			root[groupName][itemName] = nlohmann::json::array({ tmp.x, tmp.y, tmp.z });
		}
		else if (std::holds_alternative<std::string>(item)) {
			root[groupName][itemName] = std::get<std::string>(item);
		}
	}

	const std::filesystem::path kDirectoryPath = dataDirectoryName_;

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directory(kDirectoryPath);
	}

	std::string groupNameTmp;
	for (auto& i : groupName) {
		if (i == '\0') {
			break;
		}
		groupNameTmp += i;
	}
	auto filePath = (kDirectoryPath.string() + groupNameTmp) + std::string(".json");

	std::ofstream file(filePath);

	if (file.fail()) {
		assert(!"fileSaveFailed");
		return;
	}

	file << std::setw(4) << root << std::endl;

	file.close();
}

void Particle::BackUpSettingFile(const std::string& groupName) {
	auto itrGroup = datas_.find(groupName);
	assert(itrGroup != datas_.end());

	nlohmann::json root;

	root = nlohmann::json::object();

	root[groupName] = nlohmann::json::object();

	for (auto itemItr = itrGroup->second.begin(); itemItr != itrGroup->second.end(); itemItr++) {
		const std::string& itemName = itemItr->first;

		auto& item = itemItr->second;

		if (std::holds_alternative<uint32_t>(item)) {
			root[groupName][itemName] = std::get<uint32_t>(item);
		}
		else if (std::holds_alternative<float>(item)) {
			root[groupName][itemName] = std::get<float>(item);
		}
		else if (std::holds_alternative<Vector2>(item)) {
			auto tmp = std::get<Vector2>(item);
			root[groupName][itemName] = nlohmann::json::array({ tmp.x, tmp.y });
		}
		else if (std::holds_alternative<Vector3>(item)) {
			auto tmp = std::get<Vector3>(item);
			root[groupName][itemName] = nlohmann::json::array({ tmp.x, tmp.y, tmp.z });
		}
		else if (std::holds_alternative<std::string>(item)) {
			root[groupName][itemName] = std::get<std::string>(item);
		}
	}

	const std::filesystem::path kDirectoryPath = dataDirectoryName_ + "BackUp/";

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directory(kDirectoryPath);
	}

	std::string groupNameTmp;
	for (auto& i : groupName) {
		if (i == '\0') {
			break;
		}
		groupNameTmp += i;
	}
	auto filePath = (kDirectoryPath.string() + "delete_" + groupNameTmp) + std::string(".json");

	std::ofstream file(filePath);

	if (file.fail()) {
		assert(!"fileSaveFailed");
		return;
	}

	file << std::setw(4) << root << std::endl;

	file.close();
}

void Particle::ParticleStart() {
	if (!settings_.empty()) {
		currentParticleIndex_ = 0;
		emitterPos = settings_[currentParticleIndex_].emitter_.pos_;
		settings_[currentSettingIndex_].isValid_ = false;
		settings_[currentSettingIndex_].isValid_.Update();
		settings_[currentParticleIndex_].isValid_ = true;
	}
}

void Particle::ParticleStart(const Vector3& pos) {
	if (!settings_.empty()) {
		currentParticleIndex_ = 0;
		emitterPos = pos;
		settings_[currentSettingIndex_].isValid_ = false;
		settings_[currentSettingIndex_].isValid_.Update();
		settings_[currentParticleIndex_].isValid_ = true;
	}
}

void Particle::ParticleStop()
{
	settings_[currentSettingIndex_].isValid_ = false;
}


void Particle::Update() {
	assert(wtfs_.size() == wvpMat_.Size());

	if (tex_ && tex_->CanUse() && !isLoad_) {
		isLoad_ = true;
	}

	// currentSettingIndexがsettingの要素数を超えてたらassertで止める
	if (currentSettingIndex_ >= settings_.size() || settings_.empty()) {
		return;
	}
	auto nowTime = std::chrono::steady_clock::now();

	if (settings_[currentSettingIndex_].isValid_) {
		settings_[currentSettingIndex_].emitter_.pos_ = emitterPos;
	}

	// 有効になった瞬間始めた瞬間を保存
	if (settings_[currentSettingIndex_].isValid_.OnEnter()) {
		settings_[currentSettingIndex_].startTime_ = nowTime;
		settings_[currentSettingIndex_].durationTime_ = nowTime;
		// パーティクルの最大数にリサイズ
		this->Resize(settings_[currentSettingIndex_].emitter_.particleMaxNum_);
	}
	// 有効中
	else if (settings_[currentSettingIndex_].isValid_.OnStay()) {
		// 最後に出した時間からのmilliseconds
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - settings_[currentSettingIndex_].durationTime_);

		// 出す頻度ランダム
		auto freq = Lamb::Random(settings_[currentSettingIndex_].freq_.first, settings_[currentSettingIndex_].freq_.second);

		// 頻度時間を超えてたら
		if (duration > decltype(duration)(freq)) {
			settings_[currentSettingIndex_].durationTime_ = nowTime;

			// パーティクルを出す数ランダム
			auto particleNum = Lamb::Random(settings_[currentSettingIndex_].particleNum_.first, settings_[currentSettingIndex_].particleNum_.second);

			// パーティクルの設定
			for (uint32_t i = currentParticleIndex_; i < currentParticleIndex_ + particleNum; i++) {
				if (i >= wtfs_.size()) {
					currentParticleIndex_ = 0u;
					break;
				}

				// ポジションランダム
				Vector3 maxPos = settings_[currentSettingIndex_].emitter_.pos_ + settings_[currentSettingIndex_].emitter_.size_;
				Vector3 minPos = settings_[currentSettingIndex_].emitter_.pos_ - settings_[currentSettingIndex_].emitter_.size_;
				[[maybe_unused]] Vector3 posRotate;
				Vector3 pos;

				// ポジションのランダム
				switch (settings_[currentSettingIndex_].emitter_.type_)
				{
				case Particle::EmitterType::Cube:
				default:
					pos = Lamb::Random(minPos, maxPos);
					break;

				case Particle::EmitterType::Circle:
					maxPos.x += settings_[currentSettingIndex_].emitter_.circleSize_;
					pos = Lamb::Random(settings_[currentSettingIndex_].emitter_.pos_, maxPos);
					posRotate = Lamb::Random(settings_[currentSettingIndex_].emitter_.rotate_.first, settings_[currentSettingIndex_].emitter_.rotate_.second);
					pos *= Mat4x4::MakeAffin(Vector3::kIdentity, posRotate, Vector3::kZero);
					break;
				}

				// 大きさランダム
				Vector2 size = Lamb::Random(settings_[currentSettingIndex_].size_.first, settings_[currentSettingIndex_].size_.second);
				if (settings_[currentSettingIndex_].isSameHW_) {
					size.y = size.x;
				}
				Vector2 sizeSecond = Lamb::Random(settings_[currentSettingIndex_].sizeSecond_.first, settings_[currentSettingIndex_].sizeSecond_.second);
				if (settings_[currentSettingIndex_].isSameHW_) {
					sizeSecond.y = sizeSecond.x;
				}

				// 速度ランダム
				Vector3 velocity = Lamb::Random(settings_[currentSettingIndex_].velocity_.first, settings_[currentSettingIndex_].velocity_.second);
				Vector3 velocitySecond = Lamb::Random(settings_[currentSettingIndex_].velocitySecond_.first, settings_[currentSettingIndex_].velocitySecond_.second);

				// 移動方向ランダム
				Vector3 moveRotate = Lamb::Random(settings_[currentSettingIndex_].moveRotate_.first, settings_[currentSettingIndex_].moveRotate_.second);

				// 速度回転
				velocity *= Mat4x4::MakeAffin(Vector3::kIdentity, moveRotate, Vector3::kZero);

				// 回転
				Vector3 rotate = Lamb::Random(settings_[currentSettingIndex_].rotate_.first, settings_[currentSettingIndex_].rotate_.second);
				Vector3 rotateSecond = Lamb::Random(settings_[currentSettingIndex_].rotateSecond_.first, settings_[currentSettingIndex_].rotateSecond_.second);

				// 死ぬ時間ランダム
				uint32_t deathTime = Lamb::Random(settings_[currentSettingIndex_].death_.first, settings_[currentSettingIndex_].death_.second);

				// カラー
				uint32_t color = settings_[currentSettingIndex_].color_.first;

				// ステータスセット
				wtfs_[i].pos_ = pos;
				wtfs_[i].scale_ = size;
				wtfs_[i].scaleStart_ = size;
				wtfs_[i].scaleSecond_ = sizeSecond;
				wtfs_[i].rotate_ = rotate;
				wtfs_[i].rotateStart_ = rotate;
				wtfs_[i].rotateSecond_ = rotateSecond;
				wtfs_[i].movePos_ = velocity;
				wtfs_[i].movePosSecond_ = velocitySecond;
				wtfs_[i].deathTime_ = std::chrono::milliseconds(deathTime);
				wtfs_[i].startTime_ = nowTime;
				wtfs_[i].isActive_ = true;
				wtfs_[i].color_ = color;
			}

			// インデックスを更新
			currentParticleIndex_ = currentParticleIndex_ + particleNum;
			if (currentParticleIndex_ >= wtfs_.size()) {
				currentParticleIndex_ = 0u;
			}
		}
	}


	for (auto& wtf : wtfs_) {
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - wtf.startTime_);
		if (wtf.isActive_) {
			if (duration > wtf.deathTime_) {
				wtf.isActive_ = false;
			}
			else {
				float wtfT =static_cast<float>(duration.count()) / static_cast<float>(wtf.deathTime_.count());
				Vector3 moveVec = Vector3::Lerp(wtf.movePos_, wtf.movePosSecond_, settings_[currentSettingIndex_].moveEase_(wtfT));
				wtf.pos_ += moveVec * Lamb::DeltaTime();
				wtf.color_ = ColorLerp(settings_[currentSettingIndex_].color_.first, settings_[currentSettingIndex_].color_.second, settings_[currentSettingIndex_].colorEase_(wtfT));
			
				wtf.scale_ = Vector2::Lerp(wtf.scaleStart_, wtf.scaleSecond_, settings_[currentSettingIndex_].sizeEase_(wtfT));
				
				wtf.rotate_ = Vector3::Lerp(wtf.rotateStart_, wtf.rotateSecond_, settings_[currentSettingIndex_].rotateEase_(wtfT));
			}
		}

	}

	//settings_[currentSettingIndex_].isValid_.Update();

	// もし今の設定の有効時間を過ぎていたら終了
	if (settings_[currentSettingIndex_].isValid_ && 
		settings_[currentSettingIndex_].emitter_.validTime_ <
		std::chrono::duration_cast<std::chrono::milliseconds>(
			nowTime - settings_[currentSettingIndex_].startTime_
		))
	{
		settings_[currentSettingIndex_].isValid_ = false;

		currentSettingIndex_++;
		if (currentSettingIndex_ >= settings_.size()) {
			currentSettingIndex_ = 0;
			if (isLoop_) {
				settings_[currentSettingIndex_].isValid_ = true;
			}
		}
	}
}

void Particle::Draw(
	const Vector3& cameraRotate,
	const Mat4x4& viewProjection,
	Pipeline::Blend blend
) {
	if (tex_ && isLoad_ && !settings_.empty()) {
		const Vector2& uv0 = { uvPibot.x, uvPibot.y + uvSize.y }; const Vector2& uv1 = uvSize + uvPibot;
		const Vector2& uv2 = { uvPibot.x + uvSize.x, uvPibot.y }; const Vector2& uv3 = uvPibot;

		std::array<VertexData, 4> pv = {
			Vector3{ -0.5f,  0.5f, 0.1f }, uv3,
			Vector3{  0.5f,  0.5f, 0.1f }, uv2,
			Vector3{  0.5f, -0.5f, 0.1f }, uv1,
			Vector3{ -0.5f, -0.5f, 0.1f }, uv0
		};

		VertexData* mappedData = nullptr;
		vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		std::copy(pv.begin(), pv.end(), mappedData);
		vertexResource_->Unmap(0, nullptr);

		UINT drawCount = 0;
		assert(wtfs_.size() == wvpMat_.Size());
		Vector3 billboardRotate;
		if (isBillboard_) {
			if (isYBillboard_) {
				billboardRotate.y = cameraRotate.y;
			}
			else {
				billboardRotate = cameraRotate;
			}
		}
		else {
			billboardRotate = Vector3::kZero;
		}

		for (uint32_t i = 0; i < wvpMat_.Size();i++) {
			if (wtfs_[i].isActive_) {
				wvpMat_[drawCount] = Mat4x4::MakeAffin(
					wtfs_[i].scale_, 
					wtfs_[i].rotate_ + billboardRotate, 
					wtfs_[i].pos_) * viewProjection;
				colorBuf_[drawCount] = UintToVector4(wtfs_[i].color_);
				drawCount++;
			}
		}


		if (0 < drawCount) {
			auto commandlist = DirectXCommand::GetInstance()->GetCommandList();
			// 各種描画コマンドを積む
			graphicsPipelineState_[blend]->Use();
			tex_->Use(0);
			srvHeap_->Use(wvpMat_.GetHandleUINT(), 1);
			commandlist->IASetVertexBuffers(0, 1, &vertexView_);
			commandlist->IASetIndexBuffer(&indexView_);
			commandlist->DrawIndexedInstanced(6, drawCount, 0, 0, 0);
		}
	}
}

void Particle::Debug(const std::string& guiName) {
	if (isClose_) {
		return;
	}

	if (!ImGui::Begin(guiName.c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) {
		return;
	}

	if (ImGui::BeginMenu("Load")) {
		const std::filesystem::path kDirectoryPath = "./Resources/Datas/Particles/";

		if (std::filesystem::exists(kDirectoryPath)) {
			std::filesystem::directory_iterator dirItr(kDirectoryPath);

			// directory内のファイルをすべて読み込む
			for (const auto& entry : dirItr) {
				if (ImGui::Button(entry.path().string().c_str())) {
					int32_t id = MessageBoxA(
						WindowFactory::GetInstance()->GetHwnd(),
						"Are you sure you wanna load this setting?", "Particle",
						MB_OKCANCEL | MB_ICONINFORMATION
					);

					if (id == IDOK) {
						LoadSettingDirectory(entry.path().stem().string());
						MessageBoxA(
							WindowFactory::GetInstance()->GetHwnd(),
							"Load success", "Particle",
							MB_OK | MB_ICONINFORMATION
						);
					}
				}
			}
		}

		ImGui::EndMenu();
	}
	if (ImGui::Button("Setting Add")) {
		settings_.push_back(Setting{});

	}
	for (auto i = 0llu; i < settings_.size(); i++) {
		if (!ImGui::BeginMenu(("setting" + std::to_string(i)).c_str())) {
			continue;
		}

		// エミッターの設定
		if (ImGui::TreeNode("Emitter")) {
			ImGui::DragFloat3("pos", &settings_[i].emitter_.pos_.x, 0.01f);
			emitterPos = settings_[i].emitter_.pos_;
			ImGui::DragFloat3("size", &settings_[i].emitter_.size_.x, 0.01f);
			int32_t type = static_cast<int32_t>(settings_[i].emitter_.type_);
			ImGui::SliderInt("type", &type, 0, 1);
			settings_[i].emitter_.type_ = static_cast<decltype(settings_[i].emitter_.type_)>(type);
			if (type == 1) {
				ImGui::DragFloat("circleSize", &settings_[i].emitter_.circleSize_, 0.01f);
				ImGui::DragFloat3("rotate first", &settings_[i].emitter_.rotate_.first.x, 0.01f);
				ImGui::DragFloat3("rotate second", &settings_[i].emitter_.rotate_.second.x, 0.01f);
			}
			int32_t particleMaxNum = static_cast<int32_t>(settings_[i].emitter_.particleMaxNum_);
			ImGui::DragInt("particleMaxNum", &particleMaxNum, 0.1f, 0);
			settings_[i].emitter_.particleMaxNum_ = uint32_t(particleMaxNum);
			settings_[i].emitter_.particleMaxNum_ = std::clamp(settings_[i].emitter_.particleMaxNum_, 1u, std::numeric_limits<uint32_t>::max());

			int32_t validTime = int32_t(settings_[i].emitter_.validTime_.count());
			ImGui::DragInt("vaild time(milliseconds)", &validTime, 1.0f, 0);
			settings_[i].emitter_.validTime_ = std::chrono::milliseconds(validTime);

			ImGui::TreePop();
		}

		// パーティクルの設定
		if (ImGui::TreeNode("Particle")) {
			if (ImGui::TreeNode("size")) {
				ImGui::Checkbox("Same height and width", settings_[i].isSameHW_.Data());
				if (settings_[i].isSameHW_) {
					ImGui::DragFloat("size min", &settings_[i].size_.first.x, 0.01f);
					ImGui::DragFloat("size max", &settings_[i].size_.second.x, 0.01f);
					ImGui::DragFloat("sizeSecond min", &settings_[i].sizeSecond_.first.x, 0.01f);
					ImGui::DragFloat("sizeSecond max", &settings_[i].sizeSecond_.second.x, 0.01f);
				}
				else {
					ImGui::DragFloat2("size first", &settings_[i].size_.first.x, 0.01f);
					ImGui::DragFloat2("size second", &settings_[i].size_.second.x, 0.01f);
					ImGui::DragFloat2("sizeSecond min", &settings_[i].sizeSecond_.first.x, 0.01f);
					ImGui::DragFloat2("sizeSecond max", &settings_[i].sizeSecond_.second.x, 0.01f);
				}
				ImGui::SliderInt("easeType", &settings_[i].sizeEaseType_, 0, 30);
				settings_[i].sizeEase_ = Easing::GetFunction(settings_[i].sizeEaseType_);

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("spd")) {
				ImGui::DragFloat3("velocity min", &settings_[i].velocity_.first.x, 0.01f);
				ImGui::DragFloat3("velocity max", &settings_[i].velocity_.second.x, 0.01f);
				ImGui::DragFloat3("velocitySecond min", &settings_[i].velocitySecond_.first.x, 0.01f);
				ImGui::DragFloat3("velocitySecond max", &settings_[i].velocitySecond_.second.x, 0.01f);

				ImGui::SliderInt("easeType", &settings_[i].moveEaseType, 0, 30);
				settings_[i].moveEase_ = Easing::GetFunction(settings_[i].moveEaseType);

				ImGui::DragFloat3("rotate min", &settings_[i].moveRotate_.first.x, 0.01f);
				ImGui::DragFloat3("rotate max", &settings_[i].moveRotate_.second.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("rotate")) {
				ImGui::DragFloat3("rotate min", &settings_[i].rotate_.first.x, 0.01f);
				ImGui::DragFloat3("rotate max", &settings_[i].rotate_.second.x, 0.01f);
				ImGui::DragFloat3("rotateSecond min", &settings_[i].rotateSecond_.first.x, 0.01f);
				ImGui::DragFloat3("rotateSecond max", &settings_[i].rotateSecond_.second.x, 0.01f);
				ImGui::SliderInt("easeType", &settings_[i].rotateEaseType_, 0, 30);
				settings_[i].moveEase_ = Easing::GetFunction(settings_[i].rotateEaseType_);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Appear")) {
				auto particleNumFirst = int32_t(settings_[i].particleNum_.first);
				auto particleNumSecond = int32_t(settings_[i].particleNum_.second);
				ImGui::DragInt("particleNum min", &particleNumFirst, 0.1f, 0, int32_t(settings_[i].emitter_.particleMaxNum_));
				ImGui::DragInt("particleNum max", &particleNumSecond, 0.1f, 0, int32_t(settings_[i].emitter_.particleMaxNum_));
				settings_[i].particleNum_.first = uint32_t(particleNumFirst);
				settings_[i].particleNum_.second = uint32_t(particleNumSecond);

				auto freqFirst = int32_t(settings_[i].freq_.first);
				auto freqSecond = int32_t(settings_[i].freq_.second);
				ImGui::DragInt("freq min(milliseconds)", &freqFirst, 1.0f, 0, int32_t(settings_[i].emitter_.validTime_.count()));
				ImGui::DragInt("freq max(milliseconds)", &freqSecond, 1.0f, 0, int32_t(settings_[i].emitter_.validTime_.count()));
				settings_[i].freq_.first = uint32_t(freqFirst);
				settings_[i].freq_.second = uint32_t(freqSecond);

				auto deathFirst = int32_t(settings_[i].death_.first);
				auto deathSecond = int32_t(settings_[i].death_.second);
				ImGui::DragInt("death min(milliseconds)", &deathFirst, 10.0f, 0, std::numeric_limits<int32_t>::max());
				ImGui::DragInt("death max(milliseconds)", &deathSecond, 10.0f, 0, std::numeric_limits<int32_t>::max());
				settings_[i].death_.first = uint32_t(deathFirst);
				settings_[i].death_.second = uint32_t(deathSecond);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Color")) {
				ImGui::SliderInt("easeType", &settings_[i].colorEaseType_, 0, 30);
				settings_[i].colorEase_ = Easing::GetFunction(settings_[i].colorEaseType_);

				Vector4 colorFirst = UintToVector4(settings_[i].color_.first);
				Vector4 colorSecond = UintToVector4(settings_[i].color_.second);
				ImGui::ColorEdit4("color first", colorFirst.m.data());
				ImGui::ColorEdit4("color second", colorSecond.m.data());
				settings_[i].color_.first = Vector4ToUint(colorFirst);
				settings_[i].color_.second = Vector4ToUint(colorSecond);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::Button("start")) {
			settings_[i].isValid_ = true;
			currentParticleIndex_ = uint32_t(i);
		}
		if (ImGui::Button("stop")) {
			settings_[i].isValid_ = false;
		}

		const auto groupName = ("setting" + std::to_string(i));

		datas_[groupName]["Emitter_Pos"] = settings_[i].emitter_.pos_;
		datas_[groupName]["Emitter_Size"] = settings_[i].emitter_.size_;
		datas_[groupName]["Emitter_Type"] = static_cast<uint32_t>(settings_[i].emitter_.type_);
		datas_[groupName]["Emitter_CircleSize"] = settings_[i].emitter_.circleSize_;
		datas_[groupName]["Emitter_CircleSize"] = settings_[i].emitter_.circleSize_;
		datas_[groupName]["Emitter_RotateFirst"] = settings_[i].emitter_.rotate_.first;
		datas_[groupName]["Emitter_RotateSecond"] = settings_[i].emitter_.rotate_.second;
		datas_[groupName]["Emitter_ParticleMaxNum"] = settings_[i].emitter_.particleMaxNum_;
		datas_[groupName]["Emitter_vaildTime"] = static_cast<uint32_t>(settings_[i].emitter_.validTime_.count());

		// 大きさ
		datas_[groupName]["Particle_isSameHW"] = static_cast<uint32_t>(settings_[i].isSameHW_);
		datas_[groupName]["Particle_size1"] = settings_[i].size_.first;
		datas_[groupName]["Particle_size2"] = settings_[i].size_.second;
		datas_[groupName]["Particle_sizeSecond1"] = settings_[i].sizeSecond_.first;
		datas_[groupName]["Particle_sizeSecond2"] = settings_[i].sizeSecond_.second;
		datas_[groupName]["Particle_sizeEase"] = static_cast<uint32_t>(settings_[i].colorEaseType_);

		// 速度
		datas_[groupName]["Particle_velocity1"] = settings_[i].velocity_.first;
		datas_[groupName]["Particle_velocity2"] = settings_[i].velocity_.second;
		datas_[groupName]["Particle_velocitySecond1"] = settings_[i].velocitySecond_.first;
		datas_[groupName]["Particle_velocitySecond2"] = settings_[i].velocitySecond_.second;
		datas_[groupName]["Particle_ease"] = static_cast<uint32_t>(settings_[i].moveEaseType);
		datas_[groupName]["Particle_moveRotateFirst"] = settings_[i].moveRotate_.first;
		datas_[groupName]["Particle_moveRotateSecond"] = settings_[i].moveRotate_.second;

		// 回転
		datas_[groupName]["Particle_rotate1"] = settings_[i].rotate_.first;
		datas_[groupName]["Particle_rotate2"] = settings_[i].rotate_.second;
		datas_[groupName]["Particle_rotateSecond1"] = settings_[i].rotateSecond_.first;
		datas_[groupName]["Particle_rotateSecond2"] = settings_[i].rotateSecond_.second;
		datas_[groupName]["Particle_rotateEase"] = static_cast<uint32_t>(settings_[i].rotateEaseType_);


		datas_[groupName]["Particle_particleNumFirst"] = settings_[i].particleNum_.first;
		datas_[groupName]["Particle_particleNumSecond"] = settings_[i].particleNum_.second;
		datas_[groupName]["Particle_freqFirst"] = settings_[i].freq_.first;
		datas_[groupName]["Particle_freqSecond"] = settings_[i].freq_.second;
		datas_[groupName]["Particle_deathFirst"] = settings_[i].death_.first;
		datas_[groupName]["Particle_deathSecond"] = settings_[i].death_.second;
		datas_[groupName]["Particle_colorFirst"] = settings_[i].color_.first;
		datas_[groupName]["Particle_colorSecond"] = settings_[i].color_.second;
		datas_[groupName]["Particle_colorEase"] = static_cast<uint32_t>(settings_[i].colorEaseType_);


		if (ImGui::Button("this setting save")) {
			SaveSettingFile(("setting" + std::to_string(i)).c_str());
			MessageBoxA(
				WindowFactory::GetInstance()->GetHwnd(),
				"save success", "Particle",
				MB_OK | MB_ICONINFORMATION
			);
		}

		if (ImGui::Button("this setting delete")) {
			int32_t id =  MessageBoxA(
				WindowFactory::GetInstance()->GetHwnd(),
				"Are you sure you wanna delete this setting?", "Particle",
				MB_OKCANCEL | MB_ICONINFORMATION
			);

			if (id == IDOK) {
				BackUpSettingFile(groupName);

				settings_.erase(settings_.begin() + i);
				datas_.erase(groupName);
				ImGui::EndMenu();

				MessageBoxA(
					WindowFactory::GetInstance()->GetHwnd(),
					"delete success", "Particle",
					MB_OK | MB_ICONINFORMATION
				);
				break;
			}
		}


		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();

	ImGui::Checkbox("isBillboard", &isBillboard_);
	ImGui::Checkbox("isYBillboard", &isYBillboard_);
	ImGui::Checkbox("isLoop", isLoop_.Data());
	if (ImGui::Button("all setting save")) {
		for (auto i = 0llu; i < settings_.size(); i++) {
			const auto groupName = ("setting" + std::to_string(i));

			datas_[groupName]["Emitter_Pos"] = settings_[i].emitter_.pos_;
			datas_[groupName]["Emitter_Size"] = settings_[i].emitter_.size_;
			datas_[groupName]["Emitter_Type"] = static_cast<uint32_t>(settings_[i].emitter_.type_);
			datas_[groupName]["Emitter_CircleSize"] = settings_[i].emitter_.circleSize_;
			datas_[groupName]["Emitter_CircleSize"] = settings_[i].emitter_.circleSize_;
			datas_[groupName]["Emitter_RotateFirst"] = settings_[i].emitter_.rotate_.first;
			datas_[groupName]["Emitter_RotateSecond"] = settings_[i].emitter_.rotate_.second;
			datas_[groupName]["Emitter_ParticleMaxNum"] = settings_[i].emitter_.particleMaxNum_;
			datas_[groupName]["Emitter_vaildTime"] = static_cast<uint32_t>(settings_[i].emitter_.validTime_.count());

			// 大きさ
			datas_[groupName]["Particle_isSameHW"] = static_cast<uint32_t>(settings_[i].isSameHW_);
			datas_[groupName]["Particle_size1"] = settings_[i].size_.first;
			datas_[groupName]["Particle_size2"] = settings_[i].size_.second;
			datas_[groupName]["Particle_sizeSecond1"] = settings_[i].sizeSecond_.first;
			datas_[groupName]["Particle_sizeSecond2"] = settings_[i].sizeSecond_.second;
			datas_[groupName]["Particle_sizeEase"] = static_cast<uint32_t>(settings_[i].colorEaseType_);

			// 速度
			datas_[groupName]["Particle_velocity1"] = settings_[i].velocity_.first;
			datas_[groupName]["Particle_velocity2"] = settings_[i].velocity_.second;
			datas_[groupName]["Particle_velocitySecond1"] = settings_[i].velocitySecond_.first;
			datas_[groupName]["Particle_velocitySecond2"] = settings_[i].velocitySecond_.second;
			datas_[groupName]["Particle_ease"] = static_cast<uint32_t>(settings_[i].moveEaseType);
			datas_[groupName]["Particle_moveRotateFirst"] = settings_[i].moveRotate_.first;
			datas_[groupName]["Particle_moveRotateSecond"] = settings_[i].moveRotate_.second;

			// 回転
			datas_[groupName]["Particle_rotate1"] = settings_[i].rotate_.first;
			datas_[groupName]["Particle_rotate2"] = settings_[i].rotate_.second;
			datas_[groupName]["Particle_rotateSecond1"] = settings_[i].rotateSecond_.first;
			datas_[groupName]["Particle_rotateSecond2"] = settings_[i].rotateSecond_.second;
			datas_[groupName]["Particle_rotateEase"] = static_cast<uint32_t>(settings_[i].rotateEaseType_);

			datas_[groupName]["Particle_particleNumFirst"] = settings_[i].particleNum_.first;
			datas_[groupName]["Particle_particleNumSecond"] = settings_[i].particleNum_.second;
			datas_[groupName]["Particle_freqFirst"] = settings_[i].freq_.first;
			datas_[groupName]["Particle_freqSecond"] = settings_[i].freq_.second;
			datas_[groupName]["Particle_deathFirst"] = settings_[i].death_.first;
			datas_[groupName]["Particle_deathSecond"] = settings_[i].death_.second;
			datas_[groupName]["Particle_colorFirst"] = settings_[i].color_.first;
			datas_[groupName]["Particle_colorSecond"] = settings_[i].color_.second;
			datas_[groupName]["Particle_colorEase"] = static_cast<uint32_t>(settings_[i].colorEaseType_);
			SaveSettingFile(("setting" + std::to_string(i)).c_str());
		}

		std::ofstream file{ dataDirectoryName_+"otherSetting.txt"};

		if (!file.fail() && isLoad_) {
			file << static_cast<bool>(isLoop_) << std::endl
				<< tex_->GetFileName() << std::endl 
				<< isBillboard_ << std::endl 
				<< isYBillboard_;
			file.close();
			MessageBoxA(
				WindowFactory::GetInstance()->GetHwnd(),
				"save success", "Particle",
				MB_OK | MB_ICONINFORMATION
			);
		}
	}

	auto fileNames = Lamb::GetFilePathFormDir("./Resources", ".png");

	if (isLoad_) {
		if (ImGui::TreeNode("png files Load")) {
			ImGui::Text(("now Texture is : " + tex_->GetFileName()).c_str());
			for (auto& fileName : fileNames) {
				if (ImGui::Button(fileName.string().c_str())) {
					this->ThreadLoadTexture(fileName.string());
				}
			}
			ImGui::TreePop();
		}
	}

	if (ImGui::Button("close")) {
		isClose_ = !isClose_;
	}

	ImGui::End();
}

void Particle::AnimationStart(float aniUvPibot) {
	if (!isAnimation_) {
		aniStartTime_ = std::chrono::steady_clock::now();
		isAnimation_ = true;
		aniCount_ = 0.0f;
		uvPibot.x = aniUvPibot;
	}
}

void Particle::AnimationPause() {
	isAnimation_ = false;
}

void Particle::AnimationRestart() {
	isAnimation_ = true;
}

void Particle::Animation(size_t aniSpd, bool isLoop, float aniUvStart, float aniUvEnd) {
	if (isAnimation_) {
		auto nowTime = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - aniStartTime_) >= std::chrono::milliseconds(aniSpd)) {
			aniStartTime_ = nowTime;
			aniCount_++;

			if (aniCount_ >= aniUvEnd) {
				if (isLoop) {
					aniCount_ = aniUvStart;
				}
				else {
					--aniCount_;
					isAnimation_ = false;
				}
			}

			uvPibot.x = aniUvStart;
			uvPibot.x += uvPibotSpd_ * aniCount_;
		}
	}
}