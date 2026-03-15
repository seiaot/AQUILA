#pragma once

#include <string>
#include <vector>

class CSVData
{
    CSVData(const CSVData&) = delete;
    void operator = (const CSVData&) = delete;

public:

    CSVData();
    ~CSVData();

    //テキストデータの先頭アドレスとサイズを指定してデータを構築する
    void Load(const char* pRawData, int RawDataSize);
    void Unload();

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

private:

    struct Element {
        unsigned short p;
        unsigned short size;
    };

    const char* m_pData;

    std::vector<Element> m_elements;

    char* m_pBuffer;
    int m_bufferSize;
};