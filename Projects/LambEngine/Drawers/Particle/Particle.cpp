#include "Particle.h"

#include "Math/Quaternion.h"

#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "imgui.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"

#include "Utils/UtilsLib/UtilsLib.h"
#include "Utils/Random/Random.h"
#include "Drawers/DrawerManager.h"

#include "../externals/nlohmann/json.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>


Particle::Particle() :
	wtfs_(),
	tex_(TextureManager::GetInstance()->GetTexture(TextureManager::kWhiteTexturePath)),
	isLoad_(false),
	isBillboard_(true),
	isYBillboard_(false),
	settings_(),
	currentSettingIndex_(0u),
	currentParticleIndex_(0u),
	isClose_{false},
	particleScale_(1.0f)
{

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
}


void Particle::LoadTexture(const std::string& fileName) {
	static TextureManager* textureManager = TextureManager::GetInstance();

	textureManager->LoadTexture(fileName);
	tex_ = textureManager->GetTexture(fileName);

	if (tex_ && !isLoad_) {
		isLoad_ = true;
	}
}


void Particle::LoadSettingDirectory(const std::string& directoryName) {
	settings_.clear();
	datas_.clear();

	dataDirectoryName_ = directoryName;
	const std::filesystem::path kDirectoryPath = "./Resources/Datas/Particles/" + directoryName + "/";

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

	std::ifstream file{ kDirectoryPath.string() + "otherSetting.txt"};

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
			tex_ = TextureManager::GetInstance()->GetTexture(TextureManager::kWhiteTexturePath);
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
	setting.sizeEase_ = Easeing::GetFunction(setting.sizeEaseType_);

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
	setting.rotateEase_ = Easeing::GetFunction(setting.rotateEaseType_);

	setting.particleNum_.first = std::get<uint32_t>(datas_[groupName.string()]["Particle_particleNumFirst"]);
	setting.particleNum_.second = std::get<uint32_t>(datas_[groupName.string()]["Particle_particleNumSecond"]);
	setting.freq_.first = std::get<uint32_t>(datas_[groupName.string()]["Particle_freqFirst"]);
	setting.freq_.second = std::get<uint32_t>(datas_[groupName.string()]["Particle_freqSecond"]);
	setting.death_.first = std::get<uint32_t>(datas_[groupName.string()]["Particle_deathFirst"]);
	setting.death_.second = std::get<uint32_t>(datas_[groupName.string()]["Particle_deathSecond"]);
	setting.color_.first = std::get<uint32_t>(datas_[groupName.string()]["Particle_colorFirst"]);
	setting.color_.second = std::get<uint32_t>(datas_[groupName.string()]["Particle_colorSecond"]);
	setting.colorEaseType_ = static_cast<int32_t>(std::get<uint32_t>(datas_[groupName.string()]["Particle_colorEase"]));
	setting.colorEase_ = Easeing::GetFunction(setting.colorEaseType_);

	setting.moveEaseType = static_cast<int32_t>(std::get<uint32_t>(datas_[groupName.string()]["Particle_ease"]));
	setting.moveEase_ = Easeing::GetFunction(setting.moveEaseType);
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

	const std::filesystem::path kDirectoryPath = "./Resources/Datas/Particles/" + dataDirectoryName_ + "/";

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

	const std::filesystem::path kDirectoryPath = "./Resources/Datas/Particles/" + dataDirectoryName_ + "/" + "BackUp/";

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

void Particle::ParticleStart(const Vector3& pos, const Vector3& size)
{
	if (!settings_.empty()) {
		currentParticleIndex_ = 0;
		emitterPos = pos;
		emitterSize = size;
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
	if (tex_ && !isLoad_) {
		isLoad_ = true;
	}

	// currentSettingIndexがsettingの要素数を超えてたらassertで止める
	if (currentSettingIndex_ >= settings_.size() || settings_.empty()) {
		return;
	}
	auto nowTime = std::chrono::steady_clock::now();

	if (settings_[currentSettingIndex_].isValid_) {
		settings_[currentSettingIndex_].emitter_.pos_ = emitterPos;
		settings_[currentSettingIndex_].emitter_.size_ = emitterSize * particleScale_;
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
					pos *= Quaternion::EulerToQuaternion(posRotate);
					break;
				}

				// 大きさランダム
				Vector2 size = Lamb::Random(settings_[currentSettingIndex_].size_.first, settings_[currentSettingIndex_].size_.second) * particleScale_;
				if (settings_[currentSettingIndex_].isSameHW_) {
					size.y = size.x;
				}
				Vector2 sizeSecond = Lamb::Random(settings_[currentSettingIndex_].sizeSecond_.first, settings_[currentSettingIndex_].sizeSecond_.second) * particleScale_;
				if (settings_[currentSettingIndex_].isSameHW_) {
					sizeSecond.y = sizeSecond.x;
				}

				// 速度ランダム
				Vector3 velocity = Lamb::Random(settings_[currentSettingIndex_].velocity_.first, settings_[currentSettingIndex_].velocity_.second) * particleScale_;
				Vector3 velocitySecond = Lamb::Random(settings_[currentSettingIndex_].velocitySecond_.first, settings_[currentSettingIndex_].velocitySecond_.second) * particleScale_;

				// 移動方向ランダム
				Vector3 moveRotate = Lamb::Random(settings_[currentSettingIndex_].moveRotate_.first, settings_[currentSettingIndex_].moveRotate_.second);

				// 速度回転
				velocity *= Quaternion::EulerToQuaternion(moveRotate);

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
	BlendType blend
) {
	if (tex_ && isLoad_ && !settings_.empty()) {
		Mat4x4 billboardMat;
		if (isBillboard_) {
			if (isYBillboard_) {
				billboardMat = Mat4x4::MakeRotateY(cameraRotate.y);
			}
			else {
				billboardMat = Mat4x4::MakeRotate(cameraRotate);
			}
		}
		else {
			billboardMat = Mat4x4::kIdentity;
		}
		
		Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
		Lamb::SafePtr tex2D_ = drawerManager->GetTexture2D();

		for (size_t i = 0; i < wtfs_.size();i++) {
			if (wtfs_[i].isActive_) {
				tex2D_->Draw(
					Mat4x4::MakeAffin(wtfs_[i].scale_, wtfs_[i].rotate_, wtfs_[i].pos_) * billboardMat,
					Mat4x4::kIdentity,
					viewProjection,
					tex_->GetHandleUINT(),
					wtfs_[i].color_,
					blend
				);
			}
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
				ImGui::Checkbox("Same height and width", settings_[i].isSameHW_.data());
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
				settings_[i].sizeEase_ = Easeing::GetFunction(settings_[i].sizeEaseType_);

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("spd")) {
				ImGui::DragFloat3("velocity min", &settings_[i].velocity_.first.x, 0.01f);
				ImGui::DragFloat3("velocity max", &settings_[i].velocity_.second.x, 0.01f);
				ImGui::DragFloat3("velocitySecond min", &settings_[i].velocitySecond_.first.x, 0.01f);
				ImGui::DragFloat3("velocitySecond max", &settings_[i].velocitySecond_.second.x, 0.01f);

				ImGui::SliderInt("easeType", &settings_[i].moveEaseType, 0, 30);
				settings_[i].moveEase_ = Easeing::GetFunction(settings_[i].moveEaseType);

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
				settings_[i].moveEase_ = Easeing::GetFunction(settings_[i].rotateEaseType_);
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
				settings_[i].colorEase_ = Easeing::GetFunction(settings_[i].colorEaseType_);

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

	if (ImGui::TreeNode("パーティクルスケール")) {
		ImGui::DragFloat("パーティクルスケール", &particleScale_, 0.01f, -10.0f, 10.0f);
		if (ImGui::Button("適用")) {
			ParticleStop();

			for (auto i = 0llu; i < settings_.size(); i++) {
				const auto groupName = ("setting" + std::to_string(i));

				settings_[i].emitter_.size_ *= particleScale_;
				settings_[i].emitter_.circleSize_ *= particleScale_;
				settings_[i].emitter_.circleSize_ *= particleScale_;

				// 大きさ
				settings_[i].size_.first *= particleScale_;
				settings_[i].size_.second *= particleScale_;
				settings_[i].sizeSecond_.first *= particleScale_;
				settings_[i].sizeSecond_.second *= particleScale_;

				// 速度
				settings_[i].velocity_.first *= particleScale_;
				settings_[i].velocity_.second *= particleScale_;
				settings_[i].velocitySecond_.first *= particleScale_;
				settings_[i].velocitySecond_.second *= particleScale_;
			}

			MessageBoxA(
				WindowFactory::GetInstance()->GetHwnd(),
				"Apply success", "Particle",
				MB_OK | MB_ICONINFORMATION
			);

			particleScale_ = 1.0f;
		}

		ImGui::TreePop();
	}
	ImGui::Checkbox("isBillboard", &isBillboard_);
	ImGui::Checkbox("isYBillboard", &isYBillboard_);
	ImGui::Checkbox("isLoop", isLoop_.data());
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

		std::ofstream file{ "./Resources/Datas/Particles/" + dataDirectoryName_ + "/" +"otherSetting.txt"};

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
					this->LoadTexture(fileName.string());
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

void Particle::Resize(uint32_t index) {
	wtfs_.resize(index);
}

void Particle::SetParticleScale(float particleScale) {
	particleScale_ = std::clamp(particleScale, -10.0f, 10.0f);
}
