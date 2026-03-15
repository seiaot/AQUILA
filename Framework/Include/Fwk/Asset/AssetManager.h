// ----------------------------------------------------
// アセット読み込み関連クラス
// 
// 
// 
// ----------------------------------------------------
#pragma once

#include <cstdint>
#include <string>

#include "Lib/LibDef.h"
#include "Asset.h"

namespace Fwk
{

class AssetManager
{
	friend class Framework;

	AssetManager(const AssetManager&) = delete;
	void operator = (const AssetManager&) = delete;

	AssetManager();
	~AssetManager();

public:

	//アセットマネージャの初期化処理
	void Init(
		const wchar_t* pRootDirName,
		uint32_t bufferSize
	);

	//アセットマネージャの終了処理
	void Term();

	//読み込み済みのアセットの破棄
	void Unload(AssetHandle* pAssetHandle);

	//アセットルートフォルダへの相対パスを返す
	const std::wstring& GetAssetsRoot()const;

	//アセットファイルの読み込み
	template <class T>
	bool LoadAsset(const wchar_t* AssetFileName,AssetHandle* pAssetHandle)
	{
		if (pAssetHandle == nullptr
		|| AssetFileName == nullptr
		|| wcslen(AssetFileName) == 0
		){
			return false;
		}

		wchar_t AssetPath[256] = {};
		wcscat_s(AssetPath, 256, m_AssetsRoot.c_str());
		wcscat_s(AssetPath, 256, AssetFileName);

		//既に存在するリソースを取得できていたらそのまま正常終了
		if (_getAlreadyExistsAssets(AssetPath, pAssetHandle)) {
			return true;
		}

		//空きのアセットスロットを取得
		int idx = _findFreeIndex();

		//空きがなければ読み込めない
		if (idx == -1)
			return false;

		T* p = new T();
		Asset* pAsset = static_cast<Asset*>(p);

		if (pAsset == nullptr) {
			SAFE_DELETE(p);
			return false;
		}

		if (pAsset->Init(AssetPath) == false) {
			pAsset->OnTerm();
			SAFE_DELETE(p);
			return false;
		}

		m_ppAssets[idx] = pAsset;
		m_ppAssets[idx]->FilePath = AssetPath;
		m_ppAssets[idx]->OnAssetReleased = [this](Asset* p) { _onAssetReleased(p); };

		pAssetHandle->Set(pAsset);

		return true;
	}

private:

	void _initRootDirectory(const wchar_t* dirName);
	int _findFreeIndex();
	bool _findDirectory(const wchar_t* pRoot, const wchar_t* FindDirName);

	bool _getAlreadyExistsAssets(const wchar_t* AssetPath, AssetHandle* pAssetHandle);

	void _onAssetReleased(Asset* pAsset);

private:
	uint32_t m_AssetBufferSize;
	Asset** m_ppAssets;
	std::wstring m_AssetsRoot;
};

}