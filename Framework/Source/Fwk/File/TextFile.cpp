#include "Fwk/File/TextFile.h"

#include <iostream>
#include <conio.h>

#include "Fwk/Framework.h"
#include "Fwk/Asset/AssetText.h"

TextFile::TextFile() 
: m_textFile()
{
	;
}
TextFile::~TextFile() {
	Unload();
}

void TextFile::Load(const char* filename) {

	wchar_t wcharBuffer[256] = { 0 };
	int wcharNum = MultiByteToWideChar(CP_UTF8, 0, filename, (int)strlen(filename), nullptr, 0);
	if (wcharNum == 0 || wcharNum >= 255) {
		return;
	}

	MultiByteToWideChar(CP_UTF8, 0, filename, (int)strlen(filename), wcharBuffer, wcharNum);
	Load(wcharBuffer);
}

void TextFile::Load(const wchar_t* filename)
{
	Unload();
	AssetManager_I->LoadAsset<Fwk::AssetText>(filename, &m_textFile);
}

void TextFile::Load(const std::string& filename) {
	Load(filename.c_str());
}

void TextFile::Load(const std::wstring& filename) {
	Load(filename.c_str());
}

void TextFile::Unload() {
	auto fwk = Framework_I;
	if (fwk == nullptr)
		return;

	AssetManager_I->Unload(&m_textFile);
}

//生データ（読み込んだままのデータ）のサイズを返す。
int TextFile::GetRawDataSize()const {

	auto pFile = m_textFile.Get<Fwk::AssetText>();
	if (pFile == nullptr)
		return 0;

	return static_cast<int>(pFile->GetSize());
}

//生データ（読み込んだままのデータ）の先頭アドレスを返す。
const char* TextFile::GetRawData()const{

	auto pFile = m_textFile.Get<Fwk::AssetText>();
	if (pFile == nullptr)
		return 0;

	return pFile->GetBuffer();
}

//生データ（読み込んだままのデータ）をコンソールに出力する
void TextFile::PrintRawData()const {
	int size = GetRawDataSize();
	const char* p = GetRawData();
	for (int i = 0; i < size; ++i) {
		std::cout << p[i];
	}
	std::cout << std::endl;
}

