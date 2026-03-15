#pragma once

#include <string>
#include "Fwk/Asset/Asset.h"

class TextFile
{
    TextFile(const TextFile&) = delete;
    void operator = (const TextFile&) = delete;

public:

    TextFile();
    virtual ~TextFile();

    virtual void Load(const char* filename);
    virtual void Load(const wchar_t* filename);
    virtual void Load(const std::string& filename);
    virtual void Load(const std::wstring& filename);

    virtual void Unload();

    //生データ（読み込んだままのデータ）のサイズを返す。
    int GetRawDataSize()const;
    //生データ（読み込んだままのデータ）の先頭アドレスを返す。
    const char* GetRawData()const;
    //生データ（読み込んだままのデータ）をコンソールに出力する
    void PrintRawData()const;

private:

    Fwk::AssetHandle m_textFile;
};