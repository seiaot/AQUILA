#include "Fwk/File/KeyValueFile.h"
#include "Lib/Util/Log.h"
#include "Lib/LibDef.h"

#include <ctype.h>
#include <iostream>
#include <conio.h>

#include "Fwk/Framework.h"
#include "Fwk/Asset/AssetText.h"

KeyValueFile::KeyValueFile()
{
	;
}

KeyValueFile::~KeyValueFile()
{
	Unload();
}

void KeyValueFile::Load(const char* filename)
{
	TextFile::Load(filename);
}

void KeyValueFile::Load(const wchar_t* filename)
{
	TextFile::Load(filename);
	mKeyValue.Load(GetRawData(), GetRawDataSize());
}

void KeyValueFile::Load(const std::string& filename)
{
	TextFile::Load(filename);
}

void KeyValueFile::Load(const std::wstring& filename)
{
	TextFile::Load(filename);
}

//解放
void KeyValueFile::Unload()
{
	TextFile::Unload();
	mKeyValue.Unload();
}

//データの要素数を返す
int KeyValueFile::GetKeyCount()const {
	return mKeyValue.GetKeyCount();
}

//指定したキーのデータを整数値として返す
int KeyValueFile::GetInt(const char* key)const {
	return mKeyValue.GetInt(key);
}

int KeyValueFile::GetInt(const string& key)const {
	return mKeyValue.GetInt(key);
}

//指定した番号のデータを実数値として返す
float KeyValueFile::GetFloat(const char* key)const {
	return mKeyValue.GetFloat(key);
}
float KeyValueFile::GetFloat(const string& key)const {
	return mKeyValue.GetFloat(key);
}

//指定した番号のデータを文字列として返す(string型に詰めて返すVer)
void KeyValueFile::GetString(const char* key, std::string* pString)const {
	mKeyValue.GetString(key,pString);
}
void KeyValueFile::GetString(const string& key, std::string* pString)const {
	mKeyValue.GetString(key,pString);
}

//指定した番号のデータを文字列として返す(配列に詰めて返すVer）
void KeyValueFile::GetString(const char* key, char* pBuffer, int bufferSize)const {
	mKeyValue.GetString(key,pBuffer,bufferSize);
}
void KeyValueFile::GetString(const string& key, char* pBuffer, int bufferSize)const {
	mKeyValue.GetString(key,pBuffer,bufferSize);
}

//CSVデータの取得
CSVData* KeyValueFile::GetCSVData(const char* key)const {
	return mKeyValue.GetCSVData(key);
}

CSVData* KeyValueFile::GetCSVData(const string& key)const {
	return mKeyValue.GetCSVData(key);
}

//解析したデータをコンソールに出力する（デバッグ用）
void KeyValueFile::PrintKeyValueData()const {
	mKeyValue.PrintKeyValueData();
}

//KeyValueDataをそのまま返す
KeyValueData* KeyValueFile::GetKeyValueData() {
	return &mKeyValue;
}