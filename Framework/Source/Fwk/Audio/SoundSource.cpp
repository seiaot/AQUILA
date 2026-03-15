#include "Fwk/Audio/SoundSource.h"
#include "Fwk/Audio/WaveSound.h"
#include "Fwk/Framework.h"

#include "Lib/LibDef.h"
#include "Lib/Util/Log.h"

SoundSource::SoundSource()
	: mp_sources(nullptr)
	, m_sourseNum(0)
	, m_useSource(0)
	, m_IsInitialized(false)
{
}

SoundSource::~SoundSource() {
	Term();
}

bool SoundSource::Init( const IWaveData& pIWaveData , unsigned short sourceNum ){

	//既に初期化済みであれば警告を出したうえで再初期化
	if (m_IsInitialized) {
		Debug::Logf("SoundSource::Init 初期化済みのSoundSourceを再初期化しました。\n");
		Term();
	}

	if (sourceNum == 0 ) {
		Debug::Logf("SoundSource::Init 音源の数に0が指定されました。\n");
		return false;
	}

	m_sourseNum = sourceNum;
	mp_sources = new Lib::Audio::SourceVoice[m_sourseNum];
	for (int i = 0; i < m_sourseNum; ++i) {
		Audio_I->CreateSourceVoice(&mp_sources[i], pIWaveData.GetWaveData());
	}
	m_useSource = 0;
	m_IsInitialized = true;

	return true;
}

void SoundSource::Term() {
	if (m_sourseNum < 1) {
		return;
	}

	for (int i = 0; i < m_sourseNum; ++i) {
		mp_sources[i].Stop();
		mp_sources[i].Term();
	}
	SAFE_DELETE_ARRAY(mp_sources);
	m_sourseNum = 0;
	m_useSource = 0;
	m_IsInitialized = false;
}

void SoundSource::Play(int PlayNum) {

	if (m_sourseNum < 1) {
		return;
	}

	mp_sources[m_useSource].Play(PlayNum);
	m_useSource = (m_useSource + 1) % m_sourseNum;
}

void SoundSource::Stop() {

	if (m_sourseNum < 1) {
		return;
	}

	for (int i = 0; i < m_sourseNum; ++i) {
		mp_sources[i].Stop();
	}

	m_useSource = 0;
}

bool SoundSource::IsPlaying()const {
	if (m_sourseNum < 1) {
		return false;
	}

	for (int i = 0; i < m_sourseNum; ++i) {
		if (mp_sources[i].IsPlaying()) {
			return true;
		}
	}

	return false;
}

void SoundSource::Pause() {

	if (m_sourseNum < 1) {
		return;
	}

	for (int i = 0; i < m_sourseNum; ++i) {
		mp_sources[i].Pause();
	}

}

bool SoundSource::IsPausing() const {
	if (m_sourseNum < 1) {
		return false;
	}

	for (int i = 0; i < m_sourseNum; ++i) {
		if (mp_sources[i].IsPausing()) {
			return true;
		}
	}

	return false;
}

void SoundSource::SetVolume(float volume) {
	if (m_sourseNum < 1) {
		return ;
	}

	for (int i = 0; i < m_sourseNum; ++i) {
		mp_sources[i].SetVolume(volume);
	}
}

float SoundSource::GetVolume()const{
	if (m_sourseNum < 1) {
		return 0.0f;
	}
	return mp_sources[0].GetVolume();
}
