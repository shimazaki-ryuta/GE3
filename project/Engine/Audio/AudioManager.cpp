#include "AudioManager.h"
#include <cassert>

AudioManager* AudioManager::GetInstance()
{
	static AudioManager instance;
	return &instance;
}

void AudioManager::Initialize(std::string directoryPath) {
	directoryPath_ = directoryPath;
	HRESULT hr = XAudio2Create(&xaudio2_,0,XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xaudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
	soundNum_ = 0;
	soundDatas_.clear();
}
uint32_t AudioManager::LoadWave(const std::string& fileName) {
	
	//読み込み済Texture
	uint32_t handle = soundNum_;
	// 読み込み済みテクスチャを検索
	auto it = std::find_if(soundDatas_.begin(), soundDatas_.end(), [&](const auto& soundData) {
		return soundData->name == fileName;
		});
	if (it != soundDatas_.end()) {
		// 読み込み済みテクスチャの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(soundDatas_.begin(), it));
		return handle;
	}
	//handle++;

	std::ifstream file;
	std::string filePath = directoryPath_ + fileName;
	file.open(filePath,std::ios_base::binary);
	assert(file.is_open());
	RiffHeader riff;
	file.read((char*)&riff,sizeof(riff));
	if (strncmp(riff.chunk.id,"RIFF",4) != 0) {
		assert(0);
	}
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	//フォーマット読み込み
	formatChunk format = {};
	file.read((char*)&format,sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt,format.chunk.size);
	//ジャンクチャンク読み飛ばし
	ChunkHeader data;
	file.read((char*)&data,sizeof(data));
	if (strncmp(data.id ,"JUNK",4) == 0) {
		file.seekg(data.size,std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}
	//データ読み込み
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	char* pBuffer = new char[data.size];
	file.read(pBuffer,data.size);
	//データをコンテナに積み込み
	std::unique_ptr<SoundDataContanor> soundData;
	soundData.reset(new SoundDataContanor);
	soundData->data.wfex = format.fmt;
	soundData->data.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData->data.bufferSize_ = data.size;
	soundData->name = fileName;
	soundDatas_.push_back(std::move(soundData));
	return ++handle - 1;
}

void AudioManager::SoundUnLoad(SoundData* soundData) {
	delete[] soundData->pBuffer;
	soundData->pBuffer = 0;
	soundData->bufferSize_ = 0;
	soundData->wfex = {};
}

void AudioManager::UnLoadAll() {
	for (std::vector<std::unique_ptr<SoundDataContanor>>::iterator ite = soundDatas_.begin(); ite != soundDatas_.end();ite++) {
		SoundUnLoad(&(*ite)->data);
		ite->reset();
	}
	soundDatas_.clear();
	soundNum_ = 0;
}

void AudioManager::PlayWave(uint32_t audioHandle) {
	HRESULT hr;
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xaudio2_->CreateSourceVoice(&pSourceVoice, &(soundDatas_[audioHandle]->data.wfex));
	assert(SUCCEEDED(hr));

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundDatas_[audioHandle]->data.pBuffer;
	buf.AudioBytes = soundDatas_[audioHandle]->data.bufferSize_;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	hr = pSourceVoice->Start();
}