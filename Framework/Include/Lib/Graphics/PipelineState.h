#pragma once

#include <d3d12.h>

#include "ComPtr.h"

namespace Lib {
namespace Graphics {

//前方宣言
class DescriptorHandle;
class DescriptorPool;
class ResShader;

class PipelineState
{
    PipelineState(const PipelineState&) = delete;
    void operator = (const PipelineState&) = delete;

public:
    PipelineState();
    ~PipelineState();

    bool Init(
        ID3D12Device* pDevice,
        ID3D12RootSignature* pRootSignature,
        ResShader* pVtxShader,
        ResShader* pPixShader,
        D3D12_PRIMITIVE_TOPOLOGY_TYPE topology,
        const D3D12_BLEND_DESC& descBS,
        const D3D12_DEPTH_STENCIL_DESC& descDSS
    );

    void Term();

    ID3D12PipelineState* Get();

private:
    ComPtr<ID3D12PipelineState> m_pPipelineState; //パイプラインステート
};

}//namespace Graphics
}//namespace Lib