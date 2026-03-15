#include "Lib/Graphics/RootSignature.h"

namespace Lib {
namespace Graphics {

RootSignature::RootSignature()
	: m_pRootSignature(nullptr)
{
	;
}

RootSignature::~RootSignature()
{
	Term();
}

bool RootSignature::Init
(
	ID3D12Device* pDevice,
	D3D12_FILTER filter
)
{
	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	//ルートパラメータの設定

	D3D12_ROOT_PARAMETER param[2] = {};

	//パラメータ0番はコンスタントバッファビューを設定する
	param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	param[0].Descriptor.ShaderRegister = 0;
	param[0].Descriptor.RegisterSpace = 0;
	param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	//パラメータ1番はディスクリプタテーブル(シェーダリソースビュー)
	//※テクスチャ用
	D3D12_DESCRIPTOR_RANGE ranges[1];
	{
		D3D12_DESCRIPTOR_RANGE range = {};
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range.NumDescriptors = 1;
		range.BaseShaderRegister = 0;
		range.RegisterSpace = 0;
		range.OffsetInDescriptorsFromTableStart = 0;

		ranges[0] = range;
	}
	param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	param[1].DescriptorTable.NumDescriptorRanges = 1;
	param[1].DescriptorTable.pDescriptorRanges = ranges;
	param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//スタティックサンプラー設定
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = filter;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS;
	sampler.MaxAnisotropy = 1;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	sampler.MinLOD = -D3D12_FLOAT32_MAX;
	sampler.MaxLOD = +D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//ルートシグニチャの設定
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = 2;
	desc.NumStaticSamplers = 1;
	desc.pParameters = param;
	desc.pStaticSamplers = &sampler;
	desc.Flags = flag;

	ComPtr<ID3DBlob> pBlob;
	ComPtr<ID3DBlob> pErrorBlob;

	//シリアライズ
	auto hr = D3D12SerializeRootSignature(
		&desc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		pBlob.GetAddressOf(),
		pErrorBlob.GetAddressOf()
	);

	if (FAILED(hr))
		return false;

	//ルートシグニチャを作成
	hr = pDevice->CreateRootSignature(
		0,
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		IID_PPV_ARGS(m_pRootSignature.ReleaseAndGetAddressOf())
	);

	if (FAILED(hr))
		return false;

	return true;
}

void RootSignature::Term()
{
	m_pRootSignature.Reset();
}

ID3D12RootSignature* RootSignature::Get()
{
	return m_pRootSignature.Get();
}

}//namespace Graphics
}//namespace Lib