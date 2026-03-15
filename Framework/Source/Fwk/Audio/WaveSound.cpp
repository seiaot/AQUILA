#include "Fwk/Audio/WaveSound.h"

#include <iostream>
#include <conio.h>

#include "Fwk/Framework.h"
#include "Fwk/Asset/AssetWave.h"

WaveSound::WaveSound() 
: m_waveFile()
{
	;
}
WaveSound::~WaveSound() {
	Unload();
}

void WaveSound::Load(const char* filename) {

	wchar_t wcharBuffer[256] = { 0 };
	int wcharNum = MultiByteToWideChar(CP_UTF8, 0, filename, (int)strlen(filename), nullptr, 0);
	if (wcharNum == 0 || wcharNum >= 255) {
		return;
	}

	MultiByteToWideChar(CP_UTF8, 0, filename, (int)strlen(filename), wcharBuffer, wcharNum);
	Load(wcharBuffer);
}

void WaveSound::Load(const wchar_t* filename)
{
	Unload();
	AssetManager_I->LoadAsset<Fwk::AssetWave>(filename, &m_waveFile);
}

void WaveSound::Load(const std::string& filename) {
	Load(filename.c_str());
}

void WaveSound::Load(const std::wstring& filename) {
	Load(filename.c_str());
}

void WaveSound::Unload() {
	auto fwk = Framework_I;
	if (fwk == nullptr)
		return;

	AssetManager_I->Unload(&m_waveFile);
}

const Lib::Audio::WaveData* WaveSound::GetWaveData()const {

	auto pFile = m_waveFile.Get<Fwk::AssetWave>();
	if (pFile == nullptr)
		return nullptr;

	return pFile->GetWaveData();

}

