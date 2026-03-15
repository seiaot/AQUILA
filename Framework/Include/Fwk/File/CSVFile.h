#pragma once

#include <string>
#include "TextFile.h"
#include "Fwk/Data/CSVData.h"

class CSVFile : public TextFile
{
    CSVFile(const CSVFile&) = delete;
    void operator = (const CSVFile&) = delete;

public:

    CSVFile();
    ~CSVFile();

    void Load(const char* filename)override;
    void Load(const wchar_t* filename)override;
    void Load(const std::string& filename)override;
    void Load(const std::wstring& filename)override;

    void Unload() override;

    //データの要素数を返す
    int GetElementCount()const;

    //指定した番号のデータを整数値として返す
    int GetInt(int index)const;
    //指定した番号のデータを実数値として返す
    float GetFloat(int index)const;
    //指定した番号のデータを文字列として返す(string型に詰めて返すVer)
    void GetString(int index, std::string* pString )const;
    //指定した番号のデータを文字列として返す(配列に詰めて返すVer）
    void GetString(int index, char* pBuffer , int bufferSize )const;

    //解析したデータをコンソールに出力する（デバッグ用）
    void PrintCSVData()const;

    //CSVDataをそのまま返す
    CSVData* GetCSVData();

private:

    CSVData mCSV;

};