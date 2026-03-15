#pragma once

#include <string>
#include "Lib/Graphics/TextureSRV.h"
#include "Fwk/Asset/Asset.h"

using namespace Lib::Graphics;

class Texture
{
    Texture(const Texture&) = delete;
    void operator = (const Texture&) = delete;

public:

    Texture();
    ~Texture();

    void Load(const char* filename);
    void Load(const wchar_t* filename);
    void Load(const std::string& filename);
    void Load(const std::wstring& filename);

    void Unload();
    TextureSRV* GetSRV()const;
    UINT64 GetWidth()const;
    INT GetHeight()const;

private:

    Fwk::AssetHandle m_Texture;
};