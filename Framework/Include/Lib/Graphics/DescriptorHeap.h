#pragma once

//ディスクリプタヒープ
//ヒープからディスクリプタを１つ１つ取り出す機能をつけてある

#include <d3d12.h>
#include <atomic>

#include "ComPtr.h"
#include "Pool.h"

namespace Lib {
namespace Graphics {

class DescriptorHandle {

public:
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;

	bool HasCPU()const { return HandleCPU.ptr != 0; }
	bool HasGPU()const { return HandleGPU.ptr != 0; }
};

class DescriptorHeap
{
	DescriptorHeap(const DescriptorHeap&) = delete;
	void operator=(const DescriptorHeap&) = delete;

	DescriptorHeap();
	~DescriptorHeap();

public:
	//生成処理
	static bool Create(
		ID3D12Device* pDevice,
		const D3D12_DESCRIPTOR_HEAP_DESC* pDesc,
		DescriptorHeap** ppHeap
	);

	//参照カウンタを増やす
	void AddRef();

	//解放処理
	void Release();

	//参照カウントを取得
	uint32_t GetCount()const;

	//ディスクリプタハンドルを割り当てる
	DescriptorHandle* AllocHandle();

	//ディスクリプタハンドルを解放する
	void FreeHandle(DescriptorHandle*& pHandle);

	//利用可能なハンドル数を戻す
	uint32_t GetAvailableHandleCount()const;

	//割り当て済みのハンドルの数
	uint32_t GetAllocatedHandleCount()const;

	//ハンドルの総数を返す
	uint32_t GetHandleCount()const;

	//ディスクリプタヒープを取得する
	ID3D12DescriptorHeap* Get();

private:

	std::atomic<uint32_t> m_RefCount;
	Pool<DescriptorHandle> m_Pool;
	ComPtr<ID3D12DescriptorHeap> m_pHeap;
	uint32_t m_DescriptorSize;
};

}//namespace Graphics
}//namespace Lib