#pragma once

#include <d3d12.h>
#include "ComPtr.h"

//dxtk -------------------------------
#include <ResourceUploadBatch.h>
//------------------------------------

namespace Lib {
namespace Graphics {

class DescriptorHandle;
class DescriptorHeap;

//テクスチャから生成したシェーダリソースビューです

class TextureSRV
{
public:

	TextureSRV();
	~TextureSRV();

	bool Init(
		ID3D12Device* pDevice,
		ID3D12CommandQueue* pCommandQueue,
		DescriptorHeap* pHeapSRV,
		const wchar_t* filename
	);

	bool Init(
		ID3D12Device* pDevice,
		DescriptorHeap* pHeapSRV,
		const D3D12_RESOURCE_DESC* pDesc,
		bool isCube);

	void Term();

	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;

	bool GetDesc(D3D12_RESOURCE_DESC* pDesc);

private:
	TextureSRV(const TextureSRV&) = delete;      // アクセス禁止.
	void operator = (const TextureSRV&) = delete;      // アクセス禁止.

	D3D12_SHADER_RESOURCE_VIEW_DESC GetViewDesc(bool isCube);
private:

	ComPtr<ID3D12Resource> m_pResTex;
	DescriptorHandle* m_pHandle;
	DescriptorHeap* m_pHeap;
};

}//namespace Graphics
}//namespace Lib