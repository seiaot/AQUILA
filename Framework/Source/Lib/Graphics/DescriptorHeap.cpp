#include "Lib/Graphics/DescriptorHeap.h"

namespace Lib {
namespace Graphics {

DescriptorHeap::DescriptorHeap()
	: m_RefCount(1)
	, m_Pool()
	, m_pHeap()
	, m_DescriptorSize(0)
{

}

DescriptorHeap::~DescriptorHeap()
{
	m_Pool.Term();
	m_pHeap.Reset();
	m_DescriptorSize = 0;
}

//生成処理
bool DescriptorHeap::Create(
	ID3D12Device* pDevice,
	const D3D12_DESCRIPTOR_HEAP_DESC* pDesc,
	DescriptorHeap** ppHeap
) {
	//引数のチェック
	if (pDevice == nullptr)
		return false;
	if (pDesc == nullptr)
		return false;
	if (ppHeap == nullptr)
		return false;

	//インスタンスを作成します
	auto instance = new(std::nothrow) DescriptorHeap();
	if (instance == nullptr)
		return false;

	//ディスクリプタヒープを作成
	auto hr = pDevice->CreateDescriptorHeap(pDesc, IID_PPV_ARGS(instance->m_pHeap.GetAddressOf()));

	if (FAILED(hr))
	{
		instance->Release();
		return false;
	}

	//プールを初期化する
	if (!instance->m_Pool.Init(pDesc->NumDescriptors)) {
		instance->Release();
		return false;
	}

	//ディスクリプタの加算サイズを取得する
	instance->m_DescriptorSize = pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);

	//インスタンスを設定
	*ppHeap = instance;

	return true;
}

//参照カウンタを増やす
void DescriptorHeap::AddRef()
{
	++m_RefCount;
}

//解放処理
void DescriptorHeap::Release()
{
	--m_RefCount;
	if (m_RefCount == 0) {
		delete this;
	}
}

//参照カウントを取得
uint32_t DescriptorHeap::GetCount()const
{
	return m_RefCount;
}

//ディスクリプタハンドルを割り当てる
DescriptorHandle* DescriptorHeap::AllocHandle()
{
	auto func = [&](uint32_t index, DescriptorHandle* pHandle)
	{
		auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += (size_t)(m_DescriptorSize * index);

		auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += (size_t)(m_DescriptorSize * index);

		pHandle->HandleCPU = handleCPU;
		pHandle->HandleGPU = handleGPU;
	};

	return m_Pool.Alloc(func);

}

//ディスクリプタハンドルを解放する
void DescriptorHeap::FreeHandle(DescriptorHandle*& pHandle)
{
	if (pHandle != nullptr)
	{
		m_Pool.Free(pHandle);
		pHandle = nullptr;
	}
}

//利用可能なハンドル数を戻す
uint32_t DescriptorHeap::GetAvailableHandleCount()const
{
	return m_Pool.GetAvailableCount();
}

//割り当て済みのハンドルの数
uint32_t DescriptorHeap::GetAllocatedHandleCount()const
{
	return m_Pool.GetUsedCount();
}

//ハンドルの総数を返す
uint32_t DescriptorHeap::GetHandleCount()const
{
	return m_Pool.GetSize();
}

//ディスクリプタヒープを取得する
ID3D12DescriptorHeap* DescriptorHeap::Get()
{
	return m_pHeap.Get();
}

}//namespace Graphics
}//namespace Lib