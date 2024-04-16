#include "AudioManager.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include <cassert>
#include <filesystem>
#include "Error/Error.h"
#include "Utils/SafeDelete/SafeDelete.h"

Lamb::SafePtr<AudioManager> AudioManager::instance_ = nullptr;
void AudioManager::Inititalize() {
	instance_.reset(new AudioManager());
}
void AudioManager::Finalize() {
	instance_.reset();
}

AudioManager::AudioManager() :
	xAudio2_(),
	masterVoice_(nullptr),
	audios_{},
	threadAudioBuff_{},
	load_{},
	mtx_{},
	isThreadLoadFinish_{false}
{
	// Media Foundationの初期化
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);


	HRESULT hr = XAudio2Create(xAudio2_.GetAddressOf(), 0u, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<AudioManager>("XAudio2Create()", "Constructor");
	}

	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<AudioManager>("CreateMasteringVoicey()", "Constructor");
	}

	Lamb::AddLog("Initialize AudioManager succeeded");
}
AudioManager::~AudioManager() {
	xAudio2_.Reset();
	MFShutdown();
	if (load_.joinable()) {
		load_.join();
	}
}

Audio* const AudioManager::LoadWav(const std::string& fileName) {
	if (!std::filesystem::exists(std::filesystem::path(fileName))) {
		throw Lamb::Error::Code<AudioManager>("There is not this file -> " + fileName, __func__);
	}


	auto itr = audios_.find(fileName);

	if (itr == audios_.end()) {
		auto audio = std::make_unique<Audio>();
		audio->Load(fileName);
		audios_.insert({ fileName, std::move(audio) });
	}

	return audios_[fileName].get();
}

void AudioManager::LoadWav(const std::string& fileName, class Audio** const audio) {
	// コンテナに追加
	threadAudioBuff_.push({fileName, audio});
}

void AudioManager::Unload(const std::string& fileName)
{
	auto isExisit = audios_.find(fileName);

	if (isExisit != audios_.end()) {
		audios_.erase(isExisit);
	}
}

void AudioManager::Unload(Audio* audio)
{
	if (!audio) {
		return;
	}

	auto isExisit = audios_.find(audio->fileName_);

	if (isExisit != audios_.end()) {
		audios_.erase(isExisit);
	}
}

void AudioManager::ThreadLoad() {
	if (!threadAudioBuff_.empty() && !load_.joinable()) {
		auto loadProc = [this]() {
			std::lock_guard<std::mutex> lock(mtx_);
			while (!threadAudioBuff_.empty()) {
				if (Lamb::IsEngineFianlize()) {
					break;
				}

				auto& front = threadAudioBuff_.front();

				auto audio = audios_.find(front.fileName_);
				if (audio == audios_.end()) {;
					audios_.insert(std::make_pair(front.fileName_, std::make_unique<Audio>()));
					audios_[front.fileName_]->Load(front.fileName_);
					*front.audio_ = audios_[front.fileName_].get();
				}
				else {
					*front.audio_ = audio->second.get();
				}

				threadAudioBuff_.pop();
			}

			isThreadLoadFinish_ = true;
			};

		load_ = std::thread{ loadProc };
	}
}

void AudioManager::JoinThread() {
	if (load_.joinable() && threadAudioBuff_.empty()) {
		load_.join();
	}
}

void AudioManager::CheckThreadLoadFinish() {
	if (isThreadLoadFinish_ && threadAudioBuff_.empty()) {
		JoinThread();
		isThreadLoadFinish_ = false;
	}
}