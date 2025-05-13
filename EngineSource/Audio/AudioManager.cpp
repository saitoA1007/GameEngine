#include"AudioManager.h"
#include<cassert>
using namespace GameEngine;

AudioManager::~AudioManager() {}

void AudioManager::Finalize() {
	xAudio2_.Reset();
	SoundUnload();
}

void AudioManager::Initialize() {

	// XAudioエンジンのインスタンスを生成
	HRESULT result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// マスターボイスを生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(result));
}

void AudioManager::SoundPlayWave(const SoundData& soundData) {
	HRESULT result;

	// 波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

AudioManager::SoundData AudioManager::SoundLoadWave(const std::string& filename) {
	//HRESULT result;

	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVRかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	//// Forrmatチャンクの読み込み
	//FormatChunk format = {};
	//// チャンクヘッダーの確認
	//file.read((char*)&format, sizeof(ChunkHeader));
	//if (strncmp(format.chunk.id, "fmt", 4) != 0) {
	//	assert(0);
	//}
	// チャンク本体の読み込み
	//assert(format.chunk.size <= sizeof(format.fmt));
	//file.read((char*)&format.fmt, format.chunk.size);

	FormatChunk format = {};
	ChunkHeader chunk = {};

	// チャンクを順に読みながらfmtチャンクを探す
	while (file.read((char*)&chunk, sizeof(chunk))) {
		if (strncmp(chunk.id, "fmt ", 4) == 0) {
			// サイズが適正か確認
			assert(chunk.size <= sizeof(format.fmt));
			format.chunk = chunk;
			file.read((char*)&format.fmt, chunk.size);
			break;
		} else {
			// 必要のないチャンクは読み飛ばす
			file.seekg(chunk.size, std::ios_base::cur);
		}
	}

	// fmtチャンクが見つからなければエラー
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}


	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));

	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	// Waveファイルを閉じる
	file.close();

	// returnする為の音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData.name = filename;

	return soundData;
}

void AudioManager::SoundUnload() {

	for (uint32_t i = 0; i < soundData_.size(); ++i) {
		// バッファのメモリを解放
		delete[] soundData_.at(i).pBuffer;

		soundData_.at(i).pBuffer = 0;
		soundData_.at(i).bufferSize = 0;
		soundData_.at(i).wfex = {};
	}
}

uint32_t AudioManager::Load(const std::string& fileName) {

	// データ数が0でなく、同じ音声データがあればその配列番号を返す
	if (soundData_.size() != 0) {
		for (uint32_t i = 0; i < soundData_.size(); ++i) {
			if (soundData_.at(i).name == fileName) {
				return i;
			}
		}
	}

	// 音声データをロード
	soundData_.push_back(SoundLoadWave(fileName));

	// 読み込んだデータが格納されている配列番号を返す
	return uint32_t(soundData_.size() - 1);
}

void AudioManager::Play(uint32_t soundHandle) {

	// 音声を再生
	SoundPlayWave(soundData_.at(soundHandle));
}