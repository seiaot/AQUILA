#include "Lib/Graphics/TextureSRV.h"

#include "Lib/Graphics/DescriptorHeap.h"
#include "Lib/Util/Log.h"

//dxtk
#include "WICTextureLoader.h"
#include "ResourceUploadBatch.h"
#include "DDSTextureLoader.h"
#include "VertexTypes.h"
#pragma comment( lib, "DirectXTK12.lib" )

#include <locale.h>
#include <wchar.h>

using namespace DirectX;

namespace Lib {
namespace Graphics {

TextureSRV::TextureSRV()
	: m_pResTex()
	, m_pHeap(nullptr)
	, m_pHandle(nullptr)
{
	;
}

TextureSRV::~TextureSRV()
{
	Term();
}

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool TextureSRV::Init
(
	ID3D12Device* pDevice,
	ID3D12CommandQueue* pCommandQueue,
	DescriptorHeap* pHeapSRV,
	const wchar_t* filename
)
{
	// 引数チェック.
	if (pDevice == nullptr
	|| pCommandQueue == nullptr
	|| pHeapSRV == nullptr
	|| filename == nullptr
	)
	{
		Debug::Log("Error : Invalid Argument.");
		return false;
	}

	assert(m_pHeap == nullptr);
	assert(m_pHandle == nullptr);

	// ディスクリプタプールを設定.
	m_pHeap = pHeapSRV;
	m_pHeap->AddRef();

	// ディスクリプタハンドルを取得.
	m_pHandle = m_pHeap->AllocHandle();
	if (m_pHandle == nullptr)
	{
		return false;
	}

	// ファイルからテクスチャを生成.
	bool isCube = false;

	ResourceUploadBatch batch(pDevice);
	batch.Begin();

	auto hr = DirectX::CreateWICTextureFromFile(
		pDevice
		,batch
		,filename
		, m_pResTex.GetAddressOf()
		, false
	);

	if (FAILED(hr))
	{
		return false;
	}

	// バッチ終了.
	auto future = batch.End(pCommandQueue);

	// バッチ完了を待機.
	future.wait();

	// シェーダリソースビューの設定を求める.
	auto viewDesc = GetViewDesc(isCube);

	// シェーダリソースビューを生成します.
	pDevice->CreateShaderResourceView(m_pResTex.Get(), &viewDesc, m_pHandle->HandleCPU);

	// 正常終了.
	return true;
}

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool TextureSRV::Init
(
	ID3D12Device* pDevice,
	DescriptorHeap* pHeapSRV,
	const D3D12_RESOURCE_DESC* pDesc,
	bool                        isCube
)
{
	if (pDevice == nullptr || pHeapSRV == nullptr || pDesc == nullptr)
	{
		return false;
	}

	assert(m_pHeap == nullptr);
	assert(m_pHandle == nullptr);

	// ディスクリプタプールを設定.
	m_pHeap = pHeapSRV;
	m_pHeap->AddRef();

	// ディスクリプタハンドルを取得.
	m_pHandle = m_pHeap->AllocHandle();
	if (m_pHandle == nullptr)
	{
		return false;
	}

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.CreationNodeMask = 0;
	prop.VisibleNodeMask = 0;

	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		pDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(m_pResTex.GetAddressOf())
	);
	if (FAILED(hr))
	{
		Debug::Logf("Error : ID3D12Device::CreateCommittedResource() Failed. retcode = 0x%x", hr);
		return false;
	}

	// シェーダリソースビューの設定を求める.
	auto viewDesc = GetViewDesc(isCube);

	// シェーダリソースビューを生成します.
	pDevice->CreateShaderResourceView(m_pResTex.Get(), &viewDesc, m_pHandle->HandleCPU);

	return true;
}
//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void TextureSRV::Term()
{
	m_pResTex.Reset();

	// ディスクリプタハンドルを解放.
	if (m_pHandle != nullptr && m_pHeap != nullptr)
	{
		m_pHeap->FreeHandle(m_pHandle);
		m_pHandle = nullptr;
	}

	// ディスクリプタプールを解放.
	if (m_pHeap != nullptr)
	{
		m_pHeap->Release();
		m_pHeap = nullptr;
	}
}

//-----------------------------------------------------------------------------
//      CPUディスクリプタハンドルを取得します.
//-----------------------------------------------------------------------------
D3D12_CPU_DESCRIPTOR_HANDLE TextureSRV::GetHandleCPU() const
{
	if (m_pHandle != nullptr)
	{
		return m_pHandle->HandleCPU;
	}

	return D3D12_CPU_DESCRIPTOR_HANDLE();
}

//-----------------------------------------------------------------------------
//      GPUディスクリプタハンドルを取得します.
//-----------------------------------------------------------------------------
D3D12_GPU_DESCRIPTOR_HANDLE TextureSRV::GetHandleGPU() const
{
	if (m_pHandle != nullptr)
	{
		return m_pHandle->HandleGPU;
	}

	return D3D12_GPU_DESCRIPTOR_HANDLE();
}

bool TextureSRV::GetDesc(D3D12_RESOURCE_DESC* pDesc)
{
	if (pDesc == nullptr)
		return false;

	if (m_pResTex == nullptr)
		return false;

	*pDesc = m_pResTex->GetDesc();

	return true;
}

//-----------------------------------------------------------------------------
//      シェーダリソースビューの設定を求めます.
//-----------------------------------------------------------------------------
D3D12_SHADER_RESOURCE_VIEW_DESC TextureSRV::GetViewDesc(bool isCube)
{
	auto desc = m_pResTex->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};

	viewDesc.Format = desc.Format;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (desc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_BUFFER:
	{
		// バッファは対象外とします.
		abort();    // アプリを止める.
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
	{
		if (desc.DepthOrArraySize > 1)
		{
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;

			viewDesc.Texture1DArray.MostDetailedMip = 0;
			viewDesc.Texture1DArray.MipLevels = desc.MipLevels;
			viewDesc.Texture1DArray.FirstArraySlice = 0;
			viewDesc.Texture1DArray.ArraySize = desc.DepthOrArraySize;
			viewDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
		}
		else
		{
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;

			viewDesc.Texture1D.MostDetailedMip = 0;
			viewDesc.Texture1D.MipLevels = desc.MipLevels;
			viewDesc.Texture1D.ResourceMinLODClamp = 0.0f;
		}
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
	{
		if (isCube)
		{
			if (desc.DepthOrArraySize > 6)
			{
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;

				viewDesc.TextureCubeArray.MostDetailedMip = 0;
				viewDesc.TextureCubeArray.MipLevels = desc.MipLevels;
				viewDesc.TextureCubeArray.First2DArrayFace = 0;
				viewDesc.TextureCubeArray.NumCubes = (desc.DepthOrArraySize / 6);
				viewDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;

				viewDesc.TextureCube.MostDetailedMip = 0;
				viewDesc.TextureCube.MipLevels = desc.MipLevels;
				viewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			}
		}
		else
		{
			if (desc.DepthOrArraySize > 1)
			{
				if (desc.MipLevels > 1)
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;

					viewDesc.Texture2DMSArray.FirstArraySlice = 0;
					viewDesc.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;
				}
				else
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

					viewDesc.Texture2DArray.MostDetailedMip = 0;
					viewDesc.Texture2DArray.MipLevels = desc.MipLevels;
					viewDesc.Texture2DArray.FirstArraySlice = 0;
					viewDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
					viewDesc.Texture2DArray.PlaneSlice = 0;
					viewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
				}
			}
			else
			{
				if (desc.MipLevels > 1)
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

					viewDesc.Texture2D.MostDetailedMip = 0;
					viewDesc.Texture2D.MipLevels = desc.MipLevels;
					viewDesc.Texture2D.PlaneSlice = 0;
					viewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				}
			}
		}
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
	{
		viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;

		viewDesc.Texture3D.MostDetailedMip = 0;
		viewDesc.Texture3D.MipLevels = desc.MipLevels;
		viewDesc.Texture3D.ResourceMinLODClamp = 0.0f;
	}
	break;

	default:
	{
		// 想定外
		abort();    // アプリを止める.
	}
	break;
	}

	return viewDesc;
}

}//namespace Graphics
}//namespace Lib