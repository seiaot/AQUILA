#pragma once

#include <string>
#include "Fwk/Data/CSVData.h"

using namespace std;

class KeyValueData
{
    KeyValueData(const KeyValueData&) = delete;
    void operator = (const KeyValueData&) = delete;

    enum {
        KEY_CHAR_MAX = 16,
    };

    struct Element
    {
        unsigned short pKey;
        unsigned short sizeKey;
        unsigned short pVal;
        unsigned short sizeVal;
        CSVData* csv;
        Element* pNext;
    };

public:

    KeyValueData();
    ~KeyValueData();

    //テキストデータの先頭アドレスとサイズを指定してデータを構築する
    void Load(const char* pRawData, int RawDataSize);
    void Unload();

    //データの要素数を返す
    int GetKeyCount()const;

    //指定したキーのデータを整数値として返す
    int GetInt(const char* key)const;
    int GetInt(const string& key)const;

    //指定したキーのデータを実数値として返す
    float GetFloat(const char* key)const;
    float GetFloat(const string& key)const;

    //指定したキーのデータを文字列として返す(string型に詰めて返すVer)
    void GetString(const char* key, std::string* pString )const;
    void GetString(const string& key, std::string* pString )const;

    //指定したキーのデータを文字列として返す(配列に詰めて返すVer）
    void GetString(const char* key, char* pBuffer , int bufferSize )const;
    void GetString(const string& key, char* pBuffer , int bufferSize )const;
       
    //指定したキーからCSVデータの取得
    CSVData* GetCSVData(const char* key)const;
    CSVData* GetCSVData(const string& key)const;

    //指定した番号のキーを文字列として返す
    void GetKey(int index, std::string* pString )const;
    void GetKey(int index, char* pBuffer , int bufferSize )const;

    //解析したデータをコンソールに出力する（デバッグ用）
    void PrintKeyValueData()const;

private:

    Element* _find(const char* pKey)const;

private:

    //元の文字列へのポインタ
    const char* m_pData;

    Element* m_pElements;
    int m_elementNum;

    char* m_pKeyBuffer;
    int m_keyBufferSize;

    char* m_pValBuffer;
    int m_valBufferSize;
};