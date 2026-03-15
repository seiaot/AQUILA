// ----------------------------------------------------
// アセット管理基底クラス（抽象クラス）
// 
// 
// 
// ----------------------------------------------------
#pragma once

#include "Lib/Graphics/Graphics.h"

#include <string>
#include <functional>
#include <list>

namespace Fwk
{
class Asset
{
	friend class AssetManager;
	friend class AssetHandle;

protected:

	Asset();
	virtual~Asset() ;

	void Term();
	void AddRef();

	virtual bool Init(
		const wchar_t* pFilePath
	) = 0;

	virtual void OnTerm() = 0;

	void SetOnForceTermedCallback(const std::function<void()>* func);
	void ClearOnForceTermedCallback(const std::function<void()>* func);

private:

	//強制解放(mgrからのみ呼んでいい)
	void ForceTerm();

private:

	std::wstring FilePath;
	unsigned int RefCount;

	std::list<const std::function<void()>*> OnForceTermed;

	std::function<void(Asset*)> OnAssetReleased;
};

class AssetHandle
{
	friend class AssetManager;

public:

	AssetHandle();
	~AssetHandle();

	Asset* Get();

	template<class T>
	T* Get() const{
		return static_cast<T*>(m_pAsset);
	}


private:

	void Term();
	void Set(Asset* pAsset);

	void OnAssetForceTermed();

	Asset* m_pAsset;
	std::function<void()> m_FuncOnFurceTerm;
};


}