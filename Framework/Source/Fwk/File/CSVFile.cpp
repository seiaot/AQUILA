#include "Fwk/File/CSVFile.h"
#include "Lib/Util/Log.h"
#include "Lib/LibDef.h"

#include <ctype.h>
#include <iostream>
#include <conio.h>

#include "Fwk/Framework.h"
#include "Fwk/Asset/AssetText.h"

CSVFile::CSVFile()
{
	;
}

CSVFile::~CSVFile()
{
	Unload();
}

void CSVFile::Load(const char* filename)
{
	TextFile::Load(filename);
}

void CSVFile::Load(const wchar_t* filename)
{
	TextFile::Load(filename);
	mCSV.Load(GetRawData(), GetRawDataSize());
}

void CSVFile::Load(const std::string& filename)
{
	TextFile::Load(filename);
}

void CSVFile::Load(const std::wstring& filename)
{
	TextFile::Load(filename);
}

//解放
void CSVFile::Unload()
{
	TextFile::Unload();
	mCSV.Unload();
}

//データの要素数を返す
int CSVFile::GetElementCount()const
{
	return mCSV.GetElementCount();
}

//指定した番号のデータを整数値として返す
int CSVFile::GetInt(int index)const
{
	return mCSV.GetInt(index);
}

//指定した番号のデータを実数値として返す
float CSVFile::GetFloat(int index)const
{
	return mCSV.GetFloat(index);
}

//指定した番号のデータを文字列として返す(string型に詰めて返すVer)
void CSVFile::GetString(int index, std::string* pString)const
{
	mCSV.GetString(index,pString);
}

//指定した番号のデータを文字列として返す(配列に詰めて返すVer）
void CSVFile::GetString(int index, char* pBuffer, int bufferSize)const
{
	mCSV.GetString(index,pBuffer,bufferSize);
}

void CSVFile::PrintCSVData()const {
	mCSV.PrintCSVData();
}

//CSVDataをそのまま返す
CSVData* CSVFile::GetCSVData() {
	return &mCSV;
}