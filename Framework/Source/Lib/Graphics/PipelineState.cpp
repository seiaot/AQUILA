#include "Lib/Graphics/PipelineState.h"

#include "Lib/Graphics/ResMesh.h"
#include "Lib/Graphics/ResShader.h"

namespace Lib {
namespace Graphics {

PipelineState::PipelineState()
	: m_pPipelineState(nullptr)
{
	;
}

PipelineState::~PipelineState()
{
	Term();
}

bool PipelineState::Init
(
	ID3D12Device* pDevice,
	ID3D12RootSignature* pRootSignature,
	ResShader* pVtxShader,
	ResShader* pPixShader,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topology, 
	const D3D12_BLEND_DESC& descBS,
	const D3D12_DEPTH_STENCIL_DESC& descDSS
)
{
	if (pVtxShader == nullptr || pVtxShader->Get() == nullptr)
		return false;

	if (pPixShader == nullptr || pPixShader->Get() == nullptr)
		return false;

	HRESULT hr;

	//ラスタライザーステートの設定
	D3D12_RASTERIZER_DESC descRS = {};
	descRS.FillMode = D3D12_FILL_MODE_SOLID;
	descRS.CullMode = D3D12_CULL_MODE_NONE;
	descRS.FrontCounterClockwise = FALSE;
	descRS.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	descRS.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	descRS.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	descRS.DepthClipEnable = FALSE;
	descRS.MultisampleEnable = FALSE;
	descRS.AntialiasedLineEnable = FALSE;
	descRS.ForcedSampleCount = 0;
	descRS.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	//パイプラインステートの設定
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = MeshVertex::InputLayout;
		desc.pRootSignature = pRootSignature;
		desc.VS = { pVtxShader->Get()->GetBufferPointer(),pVtxShader->Get()->GetBufferSize() };
		desc.PS = { pPixShader->Get()->GetBufferPointer(),pPixShader->Get()->GetBufferSize() };
		desc.RasterizerState = descRS;
		desc.BlendState = descBS;
		desc.DepthStencilState = descDSS;
		desc.PrimitiveTopologyType = topology;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.SampleMask = UINT_MAX;

		//パイプラインステート生成
		hr = pDevice->CreateGraphicsPipelineState(
			&desc,
			IID_PPV_ARGS(m_pPipelineState.GetAddressOf())
		);
		if (FAILED(hr))
			return false;
	}

	return true;
}

void PipelineState::Term()
{
	m_pPipelineState.Reset();
}

ID3D12PipelineState* PipelineState::Get()
{
	return m_pPipelineState.Get();
}

}//namespace Graphics
}//namespace Lib