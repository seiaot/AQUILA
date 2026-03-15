// ----------------------------------------------------
// アセット読み込み関連の関数群
// 
// 
// 
// ----------------------------------------------------
#include "Fwk/Asset/AssetManager.h"

#include "Lib/Util/Log.h"

#include <windows.h>
#include <string>
#include <io.h>

namespace Fwk
{

AssetManager::AssetManager()
	: m_AssetBufferSize(0)
	, m_ppAssets(nullptr)
	, m_AssetsRoot(L"")
{
	;
}

AssetManager::~AssetManager() {
	Term();
}
// ----------------------------------------
// アセットマネージャの初期化処理
// ----------------------------------------
void AssetManager::Init(
	const wchar_t* pRootDirName,
	uint32_t bufferSize
){
	Term();

	Debug::Logf(">>>>>>>> AssetManagerの初期化開始\n");

	if (bufferSize == 0) {
		Debug::Logf("不正なアセットバッファサイズです。 %d",bufferSize);
		return;
	}

	m_AssetBufferSize = bufferSize;
	m_ppAssets = new Asset*[bufferSize];
	for (uint32_t i = 0; i < m_AssetBufferSize; ++i) {
		m_ppAssets[i] = nullptr;
	}

	if (wcslen(pRootDirName) == 0) {
		Debug::Log("アセットディレクトリが指定されていません。\n");
		return;
	}

	_initRootDirectory(pRootDirName);

	if (m_AssetsRoot.size() > 0) {
		Debug::Logf("アセットディレクトリ設定完了  %ls\n",m_AssetsRoot.c_str());
	}
	else {
		Debug::Logf("アセットディレクトリが見つかりません。 %ls\n",pRootDirName);
	}	
	
	Debug::Logf("<<<<<<<< AssetManagerの初期化終了\n\n");

}

// ----------------------------------------
// アセットマネージャの終了処理
// ----------------------------------------
void AssetManager::Term()
{
	if (m_ppAssets != nullptr)
	{
		//未解放の全アセットを解放
		for (uint32_t i = 0; i < m_AssetBufferSize; i++) {
			if (m_ppAssets[i] == nullptr)
				continue;

			m_ppAssets[i]->ForceTerm();
			SAFE_DELETE(m_ppAssets[i])
		}
	}

	SAFE_DELETE_ARRAY(m_ppAssets);
	m_AssetBufferSize = 0;
}

// ----------------------------------------
// 読み込み済みのアセットの破棄
// ----------------------------------------
void AssetManager::Unload(AssetHandle* pAssetHandle)
{
	if (pAssetHandle == nullptr)
		return;

	pAssetHandle->Term();
}

// ----------------------------------------
// アセットルートフォルダへの相対パスを返す
// ----------------------------------------
const std::wstring& AssetManager::GetAssetsRoot()const
{
	return m_AssetsRoot;
}

// ----------------------------------------
// アセットデータのルートディレクトリの設定
// ----------------------------------------
void AssetManager::_initRootDirectory(const wchar_t* dirName) {

	wchar_t buffer[255];

	//フォルダのパスを探す(実行ディレクトリにあればそれが優先）
	if (_findDirectory(L"./*.*", dirName))
	{
		wsprintf(buffer,L"./%s/", dirName);
		m_AssetsRoot = buffer;
	}
	//実行ディレクトリにAssetsフォルダがなければ一つ上を探してみる
	else if (_findDirectory(L"../*.*", dirName))
	{
		wsprintf(buffer, L"../%s/", dirName);
		m_AssetsRoot = buffer;
	}
	//二つ上のディレクトリを探してみる
	else if (_findDirectory(L"../../*.*", dirName))
	{
		wsprintf(buffer, L"../../%s/", dirName);
		m_AssetsRoot = buffer;
	}
}

// ----------------------------------------
// 空きのアセットデータ構造体を返す
// ----------------------------------------
int AssetManager::_findFreeIndex()
{
	for (uint32_t i = 0; i < m_AssetBufferSize; i++) {
		if (m_ppAssets[i] != nullptr)
			continue;

		return i;
	}

	return -1;
}

// ----------------------------------------
// 指定した名前のディレクトリを探す
// ----------------------------------------
bool AssetManager::_findDirectory(const wchar_t* pRoot, const wchar_t* FindDirName)
{
	HANDLE hFind;
	WIN32_FIND_DATA win32fd;
	hFind = FindFirstFile(pRoot, &win32fd);

	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}

	bool result = false;

	do {
		if (!(win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;

		if (wcscmp(win32fd.cFileName, FindDirName) != 0)
			continue;

		result = true;

		break;

	} while (FindNextFile(hFind, &win32fd));

	FindClose(hFind);

	return result;
}

bool AssetManager::_getAlreadyExistsAssets(const wchar_t* AssetPath, AssetHandle* pAssetHandle) {

	//同じファイルをすでに読み込んでいないか確認
	for (uint32_t i = 0; i < m_AssetBufferSize; i++)
	{
		if (m_ppAssets[i] == nullptr)
			continue;

		if (wcscmp(m_ppAssets[i]->FilePath.c_str(), AssetPath) != 0)
			continue;

		pAssetHandle->Set(m_ppAssets[i]);

		return true;
	}
	return false;
}

void AssetManager::_onAssetReleased(Asset* pAsset) {

	for (uint32_t i = 0; i < m_AssetBufferSize; i++)
	{
		if (m_ppAssets[i] != pAsset)
			continue;

		SAFE_DELETE(m_ppAssets[i]);

		break;
	}

}

}