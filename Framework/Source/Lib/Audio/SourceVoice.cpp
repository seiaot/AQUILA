#include "Lib/Audio/SourceVoice.h"

#include "Lib/Util/Log.h"
#include "Lib/LibDef.h"

namespace Lib {
namespace Audio {

SourceVoice::SourceVoice()
	: mp_SourceVoice(nullptr)
	, mp_audioBuffer()
	, m_audioBufferSize()
	, m_bufferIndex(0)
	, m_audioReadBytes(0)
	, mp_waveData(nullptr)
	, m_isPlaying(false)
	, m_isPausing(false)
	, m_PlayNum(0)
	, m_PlayCount(0)
{
}

SourceVoice::~SourceVoice() {
	Term();
}

bool SourceVoice::Init(IXAudio2* pXAudio,const WaveData* pWaveData) {

	Term();

	// ソースボイスの作成 フォーマットのみ渡す
	HRESULT result = pXAudio->CreateSourceVoice(
		&mp_SourceVoice
		, (WAVEFORMATEX*)&pWaveData->format
		,0
		,XAUDIO2_DEFAULT_FREQ_RATIO
		,this
	);

	if (FAILED(result)){
		// SourceVoice作成失敗
		return false;
	}

	mp_waveData = pWaveData;
	m_audioReadBytes = 0;
	m_isPlaying = false;
	m_isPausing = false;
	m_PlayNum = 0;
	m_PlayCount = 0;

	for (int i = 0; i < 2; ++i) {
		m_audioBufferSize[i] = DEFAULT_AUDIO_BUFFER_SIZE;
		mp_audioBuffer[i] = new BYTE[m_audioBufferSize[i]];
	}
	m_bufferIndex = 0;

	return true;
}

void SourceVoice::Term() {
	if (mp_SourceVoice) {
		mp_SourceVoice->DestroyVoice();
		mp_SourceVoice = nullptr;
	}

	mp_waveData = nullptr;
	m_audioReadBytes = 0;
	m_isPlaying = false;
	m_isPausing = false;
	m_PlayNum = 0;
	m_PlayCount = 0;

	for (int i = 0; i < 2; ++i) {
		m_audioBufferSize[i] = 0;
		SAFE_DELETE_ARRAY(mp_audioBuffer[i])
	}
}

void SourceVoice::Play(int PlayNum) {

	if (mp_SourceVoice == nullptr) {
		return;
	}

	if (PlayNum == 0) {
		return;
	}

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (!IsPausing()) {
			m_audioReadBytes = 0;
			m_PlayNum = PlayNum;
			m_PlayCount = 0;
		}

		if (IsPlaying()) {
			return;
		}

		m_isPausing = false;
		m_isPlaying = true;
		mp_SourceVoice->Start();
	}
}

void SourceVoice::Stop() {

	if (mp_SourceVoice == nullptr) {
		return;
	}

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (!IsPlaying() && !IsPausing()) {
			return;
		}
		
		_stop();
	}
}

void SourceVoice::Pause() {

	if (mp_SourceVoice == nullptr) {
		return;
	}

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (!IsPlaying()) {
			return;
		}

		mp_SourceVoice->Stop();
		m_isPlaying = false;
		m_isPausing = true;
	}
}

bool SourceVoice::IsPlaying()const {
	//if (mp_SourceVoice == nullptr) {
	//	return false;
	//}
	//XAUDIO2_VOICE_STATE state;
	//mp_SourceVoice->GetState(&state);
	//Debug::Logf("BuffersQueued %d\n", state.BuffersQueued);
	return m_isPlaying;
}

bool SourceVoice::IsPausing()const {
	return m_isPausing;
}

void SourceVoice::SetVolume(float volume) {
	if (mp_SourceVoice == nullptr) {
		return;
	}
	mp_SourceVoice->SetVolume(volume);
}
float SourceVoice::GetVolume()const {
	if (mp_SourceVoice == nullptr) {
		return 0.0f;
	}

	float retVal = 0.0f;
	mp_SourceVoice->GetVolume(&retVal);
	return retVal;
}

void SourceVoice::OnVoiceProcessingPassStart(UINT32 BytesRequired) {
	//Debug::Logf("OnVoiceProcessingPassStart %d\n",BytesRequired);

	if (BytesRequired < 1) {
		return;
	}

	//要求されているデータ量がバッファサイズより大きかったら
	//バッファを再拡張する
	if (BytesRequired > m_audioBufferSize[m_bufferIndex]) {
		m_audioBufferSize[m_bufferIndex] = BytesRequired;
		SAFE_DELETE_ARRAY(mp_audioBuffer[m_bufferIndex]);
		mp_audioBuffer[m_bufferIndex] = new BYTE[m_audioBufferSize[m_bufferIndex]];
	}

	//SourceVoiceのQueueに詰めるバッファデータ
	XAUDIO2_BUFFER buffer = {};
    buffer.pAudioData = mp_audioBuffer[m_bufferIndex];
    buffer.AudioBytes = 0;

	//ここからバッファにWaveデータをコピーs
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		//要求サイズ分バッファに書き込むまでループ
		while (BytesRequired > 0)
		{
			//書き込み量.基本は要求された分だけ書き込む
			UINT32 writeSize = BytesRequired;
			//但しWaveデータの終端を超える量を要求された場合は
			//Waveの終端データまでの量にしておく
			if (m_audioReadBytes + writeSize > (UINT32)mp_waveData->size) {
				writeSize = (UINT32)mp_waveData->size - m_audioReadBytes;
			}
			//バッファへの書き込み実行
			memcpy_s(
				&(mp_audioBuffer[m_bufferIndex][buffer.AudioBytes]),
				m_audioBufferSize[m_bufferIndex],
				&mp_waveData->buffer[m_audioReadBytes],
				writeSize
			);

			//書き込みサイズを加算
			buffer.AudioBytes += writeSize;
			//要求された書き込みサイズから実際に書き込んだ分を引く
			BytesRequired -= writeSize;
			//バッファに書き込んだWaveデータの量（＝データ先頭からの位置）
			m_audioReadBytes += writeSize;

			//Waveデータの最後まで書き出していたら
			if (m_audioReadBytes == mp_waveData->size) {
				//再生回数が指定回数に到達したらループを抜ける（これで再生終了）
				//但し再生回数が負数だった場合は無限ループ
				if (m_PlayNum >0 && ++m_PlayCount >= m_PlayNum) {
					break;
				}
				else {
					//再生回数が指定回数に達していなければWaveデータの参照位置を先頭に戻して
					//ループ
					m_audioReadBytes = 0;
				}
			}
		}
	}

	m_bufferIndex = ( m_bufferIndex + 1 )% 2;

	//buffer.Flags = (m_audioReadBytes == mp_waveData->size) ? XAUDIO2_END_OF_STREAM : 0 ;
    //buffer.pContext = m_audioBuffer;

    mp_SourceVoice->SubmitSourceBuffer(&buffer);
}

void SourceVoice::OnVoiceProcessingPassEnd() {
	//Debug::Log("OnVoiceProcessingPassEnd\n");
}

void SourceVoice::OnStreamEnd() {
	//Debug::Log("OnStreamEnd\n");
}

void SourceVoice::OnBufferStart(void* pBufferContext) {
	//Debug::Log("OnBufferStart\n");

}

void SourceVoice::OnBufferEnd(void* pBufferContext) {
	//Debug::Log("OnBufferEnd\n");
	//XAUDIO2_VOICE_STATE state;
	//mp_SourceVoice->GetState(&state);
	//Debug::Logf("BuffersQueued %d\n", state.BuffersQueued);
	
	//if (state.BuffersQueued == 0) {
	//	mp_SourceVoice->Stop();
	//}
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_audioReadBytes == mp_waveData->size) {
			_stop();
		}
	}
}	

void SourceVoice::OnLoopEnd(void* pBufferContext) {
	Debug::Log("OnLoopEnd\n");
	_stop();
}

void SourceVoice::OnVoiceError(void* pBufferContext, HRESULT Error) {
	Debug::Log("OnVoiceError\n");
	_stop();
}

void SourceVoice::_stop() {	
	mp_SourceVoice->Stop();
	mp_SourceVoice->FlushSourceBuffers();
	m_isPlaying = false;
	m_isPausing = false;
}

}//namespace Audio
}//namespace Lib