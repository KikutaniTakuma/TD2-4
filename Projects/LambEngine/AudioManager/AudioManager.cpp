#include "AudioManager.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include <cassert>
#include <filesystem>
#include "Error/Error.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include "Engine/Graphics/ResourceManager/ResourceManager.h"

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
	audios_{}
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
}

Audio* const AudioManager::Load(const std::string& fileName) {
	if (!std::filesystem::exists(std::filesystem::path(fileName))) {
		throw Lamb::Error::Code<AudioManager>("There is not this file -> " + fileName, __func__);
	}


	auto itr = audios_.find(fileName);

	if (itr == audios_.end()) {
		auto audio = std::make_unique<Audio>();
		audio->Load(fileName);
		audios_.insert({ fileName, std::move(audio) });

		ResourceManager::GetInstance()->SetAudioResource(fileName);
	}

	return audios_[fileName].get();
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