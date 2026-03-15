#include "Fwk/Audio/MP3Sound.h"

#include <iostream>
#include <conio.h>

#include "Fwk/Framework.h"
#include "Fwk/Asset/AssetMP3.h"

MP3Sound::MP3Sound() 
: m_mp3File()
{
	;
}
MP3Sound::~MP3Sound() {
	Unload();
}

void MP3Sound::Load(const char* filename) {

	wchar_t wcharBuffer[256] = { 0 };
	int wcharNum = MultiByteToWideChar(CP_UTF8, 0, filename, (int)strlen(filename), nullptr, 0);
	if (wcharNum == 0 || wcharNum >= 255) {
		return;
	}

	MultiByteToWideChar(CP_UTF8, 0, filename, (int)strlen(filename), wcharBuffer, wcharNum);
	Load(wcharBuffer);
}

void MP3Sound::Load(const wchar_t* filename)
{
	Unload();
	AssetManager_I->LoadAsset<Fwk::AssetMP3>(filename, &m_mp3File);
}

void MP3Sound::Load(const std::string& filename) {
	Load(filename.c_str());
}

void MP3Sound::Load(const std::wstring& filename) {
	Load(filename.c_str());
}

void MP3Sound::Unload() {
	auto fwk = Framework_I;
	if (fwk == nullptr)
		return;

	AssetManager_I->Unload(&m_mp3File);
}

const Lib::Audio::WaveData* MP3Sound::GetWaveData()const {

	auto pFile = m_mp3File.Get<Fwk::AssetMP3>();
	if (pFile == nullptr)
		return nullptr;

	return pFile->GetWaveData();

}

