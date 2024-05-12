#include <fstream>
#include <filesystem>
#include "AudioManager/AudioManager.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "imgui.h"
#include "Error/Error.h"

Audio::Audio():
	wfet_(),
	pBuffer_(nullptr),
	bufferSize_(0u),
	pSourceVoice_(nullptr),
	isLoop_(false),
	isStart_(false),
	volume_(1.0f),
	isLoad_{false},
	fileName_{}
{}

Audio::~Audio() {
	Unload();
}

void Audio::Unload() {
	delete[] pBuffer_.get();

	pBuffer_ = nullptr;
	bufferSize_ = 0;
	wfet_ = {};
}

void Audio::Load(const std::string& fileName) {
	if (!std::filesystem::exists(std::filesystem::path{ fileName })) {
		throw Lamb::Error::Code<Audio>(("This file is not found -> " + fileName), ErrorPlace);
	}
	
	if (isLoad_) {
		Unload();
	}

	auto extension = std::filesystem::path(fileName).extension();

	if (extension != ".wav" and extension != ".mp3") {
		throw Lamb::Error::Code<Audio>(("This file is not supported (only ""mp3"" or ""wav"" file) -> " + fileName), ErrorPlace);
	}

	Lamb::SafePtr<IMFSourceReader> pMFSourceReader;
	Lamb::SafePtr<IMFMediaType> pMFMediaType;

	MFCreateSourceReaderFromURL(ConvertString(fileName).c_str(), NULL, &pMFSourceReader);


	MFCreateMediaType(&pMFMediaType);
	pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	pMFSourceReader->SetCurrentMediaType(
		static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM),
		nullptr, pMFMediaType.get()
	);

	pMFMediaType->Release();
	pMFMediaType = nullptr;
	pMFSourceReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pMFMediaType);

	Lamb::SafePtr<WAVEFORMATEX> pWaveFormat;
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType.get(), &pWaveFormat, nullptr);
	wfet_ = *pWaveFormat;

	std::vector<BYTE> mediaData;
	while (true)
	{
		Lamb::SafePtr<IMFSample> pMFSample{ nullptr };
		DWORD dwStreamFlags{ 0 };
		pMFSourceReader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			break;
		}

		Lamb::SafePtr<IMFMediaBuffer> pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		Lamb::SafePtr<BYTE> localBuffer = nullptr;
		DWORD cbCurrentLength{ 0 };
		pMFMediaBuffer->Lock(&localBuffer, nullptr, &cbCurrentLength);

		mediaData.resize(mediaData.size() + cbCurrentLength);
		std::memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, localBuffer.get(), cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();
	}

	pBuffer_ = new BYTE[mediaData.size()];
	std::memcpy(pBuffer_.get(), mediaData.data(), mediaData.size());

	bufferSize_ = static_cast<uint32_t>(mediaData.size() * sizeof(BYTE));

	
	CoTaskMemFree(pWaveFormat.get());
	pMFMediaType->Release();
	pMFSourceReader->Release();


	HRESULT hr = AudioManager::GetInstance()->xAudio2_->CreateSourceVoice(&pSourceVoice_, &wfet_);
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<Audio>("CreateSourceVoice() failed", ErrorPlace);
	}

	isLoad_ = true;
}


void Audio::Start(float volume, bool isLoop) {
	if (!isLoad_) {
		return;
	}

	HRESULT hr;
	volume_ = volume;

	Stop();
	isLoop_ = isLoop;
	if (!pSourceVoice_) {

		hr = AudioManager::GetInstance()->xAudio2_->CreateSourceVoice(&pSourceVoice_, &wfet_);
		XAUDIO2_BUFFER buf{};
		buf.pAudioData = pBuffer_.get();
		buf.AudioBytes = bufferSize_;
		buf.Flags = XAUDIO2_END_OF_STREAM;
		buf.LoopCount = isLoop_ ? XAUDIO2_LOOP_INFINITE : 0;

		if (!SUCCEEDED(hr)) {
			throw Lamb::Error::Code<Audio>("SubmitSourceBuffer() failed", ErrorPlace);
		}
		hr = pSourceVoice_->SubmitSourceBuffer(&buf);
	}
	hr = pSourceVoice_->Start();
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<Audio>("function is something error", ErrorPlace);
	}
	pSourceVoice_->SetVolume(volume_);

	isStart_ = true;
}

void Audio::Pause() {
	if (!isLoad_) {
		return;
	}
	if (pSourceVoice_) {
		pSourceVoice_->Stop();
		isStart_ = false;
	}
}

void Audio::ReStart() {
	if (!isLoad_) {
		return;
	}
	if (pSourceVoice_) {
		pSourceVoice_->Start();
		isStart_ = true;
	}
}

void Audio::Stop() {
	if (!isLoad_) {
		return;
	}
	if (pSourceVoice_) {
		pSourceVoice_->DestroyVoice();
	}
	pSourceVoice_ = nullptr;

	isStart_ = false;
}

void Audio::SetAudio(float volume) {
	volume_ = volume;
	if (pSourceVoice_ && isStart_) {
		pSourceVoice_->SetVolume(volume_);
	}
}

void Audio::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat("volume", &volume_, 0.001f, 0.0f, 1.0f);
	SetAudio(volume_);
	
	ImGui::Checkbox("isLoop", &isLoop_);
	if (ImGui::Button("Start")) {
		Start(volume_, isLoop_);
	}
	if (ImGui::Button("Stop")) {
		Stop();
	}
	if (ImGui::Button("ReStart")) {
		ReStart();
	}
	if (ImGui::Button("Pause")) {
		Pause();
	}

	ImGui::End();
#endif // _DEBUG
}