// ----------------------------------------------------
// アセット管理基底クラス（抽象クラス）
// 
// 
// 
// ----------------------------------------------------
#include "Fwk/Asset/Asset.h"

namespace Fwk
{

Asset::Asset() 
	: FilePath()
	, RefCount(0)
	, OnAssetReleased(nullptr)
{
	;
}

Asset::~Asset() {
	Term();
}

void Asset::Term() {

	if (RefCount == 0)
		return;

	--RefCount;
	if (RefCount > 0)
		return;

	OnTerm();

	if (OnAssetReleased != nullptr) {
		OnAssetReleased(this);
	}
}

//強制解放(mgrからのみ呼んでいい)
void Asset::ForceTerm() {

	for (auto func : OnForceTermed) {
		(*func)();
	}

	OnForceTermed.clear();

	RefCount = 0;

	OnTerm();

	if (OnAssetReleased != nullptr) {
		OnAssetReleased(this);
	}
}

void Asset::AddRef() {
	++RefCount;
}

void Asset::SetOnForceTermedCallback(const std::function<void()>* func){
	OnForceTermed.push_back(func);
}
void Asset::ClearOnForceTermedCallback(const std::function<void()>* func) {
	OnForceTermed.remove(func);
}

AssetHandle::AssetHandle() 
:m_pAsset(nullptr)
{
	m_FuncOnFurceTerm = [this]() {OnAssetForceTermed();};
}

AssetHandle::~AssetHandle()
{
	Term();
}

void AssetHandle::Term() {

	if (m_pAsset == nullptr)
		return;

	m_pAsset->ClearOnForceTermedCallback(&m_FuncOnFurceTerm);
	m_pAsset->Term();
	m_pAsset = nullptr;
}

Asset* AssetHandle::Get() {
	return m_pAsset;
}

void AssetHandle::Set(Asset* pAsset) 
{
	Term();
	m_pAsset = pAsset;
	m_pAsset->AddRef();
	m_pAsset->SetOnForceTermedCallback(&m_FuncOnFurceTerm);
}

void AssetHandle::OnAssetForceTermed() {
	m_pAsset = nullptr;
}

}