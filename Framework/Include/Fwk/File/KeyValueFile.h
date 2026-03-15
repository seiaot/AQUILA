#pragma once

#include <string>
#include "TextFile.h"
#include "Fwk/Data/KeyValueData.h"

class KeyValueFile : public TextFile
{
    KeyValueFile(const KeyValueFile&) = delete;
    void operator = (const KeyValueFile&) = delete;

public:

    KeyValueFile();
    ~KeyValueFile();

    void Load(const char* filename)override;
    void Load(const wchar_t* filename)override;
    void Load(const std::string& filename)override;
    void Load(const std::wstring& filename)override;

    void Unload() override;

    //データの要素数を返す
    int GetKeyCount()const;

    //指定したキーのデータを整数値として返す
    int GetInt(const char* key)const;
    int GetInt(const string& key)const;

    //指定した番号のデータを実数値として返す
    float GetFloat(const char* key)const;
    float GetFloat(const string& key)const;

    //指定した番号のデータを文字列として返す(string型に詰めて返すVer)
    void GetString(const char* key, std::string* pString )const;
    void GetString(const string& key, std::string* pString )const;

    //指定した番号のデータを文字列として返す(配列に詰めて返すVer）
    void GetString(const char* key, char* pBuffer , int bufferSize )const;
    void GetString(const string& key, char* pBuffer , int bufferSize )const;

    //CSVデータの取得
    CSVData* GetCSVData(const char* key)const;
    CSVData* GetCSVData(const string& key)const;

    //解析したデータをコンソールに出力する（デバッグ用）
    void PrintKeyValueData()const;

    //KeyValueDataをそのまま返す
    KeyValueData* GetKeyValueData();

private:

    KeyValueData mKeyValue;

};