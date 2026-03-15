#pragma once

#include <d3d12.h>

#include "ComPtr.h"

//RootSignatureはシェーダーが利⽤するリソースの対応関係を⽰したデータ構造

namespace Lib {
namespace Graphics {

//前方宣言
class DescriptorHandle;
class DescriptorPool;

class RootSignature
{
    RootSignature(const RootSignature&) = delete;
    void operator = (const RootSignature&) = delete;

public:
    RootSignature();
    ~RootSignature();

    bool Init(
        ID3D12Device* pDevice,
        D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR
    );

    void Term();

    ID3D12RootSignature* Get();

private:

    ComPtr<ID3D12RootSignature> m_pRootSignature; //ルートシグネチャ
};

}//namespace Graphics
}//namespace Lib