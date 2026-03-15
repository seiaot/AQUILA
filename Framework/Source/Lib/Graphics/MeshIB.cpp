#include "Lib/Graphics/MeshIB.h"

namespace Lib {
namespace Graphics {

MeshIB::MeshIB()
    : m_MaterialId(UINT32_MAX)
    , m_IndexCount(0)
{
    ;
}

MeshIB::~MeshIB()
{
    Term();
}

bool MeshIB::Init(ID3D12Device* pDevice, const ResMesh& resource)
{
    if (pDevice == nullptr)
    {
        return false;
    }

    if (!m_VB.Init(
        pDevice, sizeof(MeshVertex) * resource.Vertices.size(), resource.Vertices.data()))
    {
        return false;
    }

    if (!m_IB.Init(
        pDevice, sizeof(uint32_t) * resource.Indices.size(), resource.Indices.data()))
    {
        return false;
    }

    m_MaterialId = resource.MaterialId;
    m_IndexCount = uint32_t(resource.Indices.size());

    return true;
}

void MeshIB::Term()
{
    m_VB.Term();
    m_IB.Term();
    m_MaterialId = UINT32_MAX;
    m_IndexCount = 0;
}

void MeshIB::Draw(ID3D12GraphicsCommandList* pCmdList)
{
    auto VBV = m_VB.GetView();
    auto IBV = m_IB.GetView();
    pCmdList->IASetVertexBuffers(0, 1, &VBV);
    pCmdList->IASetIndexBuffer(&IBV);
    pCmdList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
}

uint32_t MeshIB::GetMaterialId() const
{
    return m_MaterialId;
}

VertexBuffer& MeshIB::GetVertexBuffer() {
    return m_VB;
}

IndexBuffer& MeshIB::GetIndexBuffer() {
    return m_IB;
}

void MeshIB::SetIndexNum(const int indexNum) {
    m_IndexCount = indexNum;
}

}//namespace Graphics
}//namespace Lib