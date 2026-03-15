// ----------------------------------------------------
// テキストアセットクラス
// 
// 
// 
// ----------------------------------------------------
#pragma once

#include "Asset.h"

namespace Fwk
{
class AssetText : public Asset
{
	friend class AssetManager;
	friend class AssetHandle;

	AssetText();
	virtual ~AssetText();

public:

	size_t GetSize()const ;
	const char* GetBuffer()const;

protected:

	bool Init(const wchar_t* pFilePath) override;
	void OnTerm() override;

private:

	size_t m_fileSize;
	char* m_pBuffer;
};

}
