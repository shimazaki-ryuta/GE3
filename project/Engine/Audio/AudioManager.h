#pragma once
#include <xaudio2.h>
#include <fstream>
//#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

//オーディオ管理クラス

class AudioManager
{
public:
	struct ChunkHeader {
		char id[4];
		int32_t size;
	};
	struct RiffHeader {
		ChunkHeader chunk;
		char type[4];
	};
	struct formatChunk {
		ChunkHeader chunk;
		WAVEFORMATEX fmt;
	};
	struct SoundData {
		WAVEFORMATEX wfex;
		BYTE* pBuffer;
		uint32_t bufferSize_;
	};

	struct SoundDataContanor {
		SoundData data;
		std::string name;
	};
	//インスタンス取得
	static AudioManager* GetInstance();
	//初期化
	void Initialize(std::string directoryPath = "Resources/Audio/");
	//終了処理
	void Finalize();
	//wave読み込み
	uint32_t LoadWave(const std::string& fileName);
	//音リソース読み込み(microsoftfaundation利用)
	uint32_t LoadAudio(const std::string& fileName);
	//音リソース解放
	void SoundUnLoad(SoundData* soundData);
	//音リソース全解放
	void UnLoadAll();
	//再生
	void PlayWave(uint32_t audioHandle);

private:
	AudioManager() = default;
	~AudioManager() = default;
	AudioManager(const AudioManager&) = delete;
	AudioManager operator=(const AudioManager&) = delete;
	Microsoft::WRL::ComPtr<IXAudio2> xaudio2_;
	IXAudio2MasteringVoice* masterVoice_;
	std::string directoryPath_;
	uint32_t soundNum_;//SoundDataの数
	std::vector<std::unique_ptr<SoundDataContanor>> soundDatas_;
};
