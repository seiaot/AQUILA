#pragma once

//メッシュの入力バッファ(IB)
//入力アセンブラステージに流し込む頂点・インデックスバッファを作る

#include "ResMesh.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Lib {
namespace Graphics {

class MeshIB
{
    MeshIB(const MeshIB&) = delete;     // アクセス禁止.
    void operator = (const MeshIB&) = delete;     // アクセス禁止.

public:

    MeshIB();

    virtual ~MeshIB();

    bool Init(ID3D12Device* pDevice, const ResMesh& resource);

    void Term();

    void Draw(ID3D12GraphicsCommandList* pCmdList);

    uint32_t GetMaterialId() const;

    VertexBuffer& GetVertexBuffer();
    IndexBuffer& GetIndexBuffer();

    void SetIndexNum(const int indexNum);

private:

    VertexBuffer    m_VB;               //!< 頂点バッファです.
    IndexBuffer     m_IB;               //!< インデックスバッファです.
    uint32_t        m_MaterialId;       //!< マテリアルIDです.
    uint32_t        m_IndexCount;       //!< インデックス数です.
};

}//namespace Graphics
}//namespace Lib