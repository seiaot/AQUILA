// ----------------------------------------------------
// テクスチャアセットクラス
// 
// 
// 
// ----------------------------------------------------
#pragma once

#include "Asset.h"
#include "Lib/Graphics/TextureSRV.h"

using namespace Lib::Graphics;

namespace Fwk
{
class AssetTexture : public Asset
{
	friend class AssetManager;
	friend class AssetHandle;

	AssetTexture();
	virtual ~AssetTexture();

public:

	TextureSRV* GetSRV();
	UINT64 GetWidth()const;
	INT GetHeight()const;


protected:

	bool Init(const wchar_t* pFilePath) override;
	void OnTerm() override;

private:

	TextureSRV m_TextureSRV;
	UINT64 m_Width;
	INT m_Height;
};

}