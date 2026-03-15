// ----------------------------------------------------
// モデルアセットクラス
// 
// 
// 
// ----------------------------------------------------
#include "Fwk/Asset/AssetModel.h"

#include "Lib/Graphics//Graphics.h"
#include "Lib/LibDef.h"

#include "Fwk/Framework.h"

using namespace DirectX;
using namespace Lib::Graphics;

namespace Fwk
{
AssetModel::AssetModel()
	: m_pMeshIBs(nullptr)
	, m_MeshNum(0)
{

}

bool AssetModel::Init( const wchar_t* pFilePath ) 
{
	Term();

	if (!LoadMesh(pFilePath, m_ResMeshes, m_ResMaterials))
		return false;

	IGraphics* pGraphics = RenderManager_I->GetGraphics();

	m_MeshNum = (uint32_t)m_ResMeshes.size();
	m_pMeshIBs = new MeshIB[m_MeshNum];

	for (uint32_t i = 0; i < m_MeshNum; ++i)
	{
		m_pMeshIBs[i].Init(pGraphics->GetDevice(),m_ResMeshes[i]);
	}

	return true;
}

void AssetModel::OnTerm() {
	
	for (size_t i = 0; i < m_MeshNum; ++i)
	{
		m_pMeshIBs[i].Term();
	}

	SAFE_DELETE_ARRAY(m_pMeshIBs);
	m_ResMeshes.clear();
	m_ResMaterials.clear();
	m_MeshNum = 0;
}

uint32_t AssetModel::GetMeshCount()const
{
	return m_MeshNum;
}

MeshIB* AssetModel::GetMesh(uint32_t index)
{
	if (index < 0 || index >= m_MeshNum) {
		return nullptr;
	}
	return &m_pMeshIBs[index];
}

uint32_t AssetModel::GetMaterialCount()const
{
	return (uint32_t)m_ResMaterials.size();
}

ResMaterial* AssetModel::GetMaterial(uint32_t index)
{
	if (index < 0 || index >= m_ResMaterials.size()) {
		return nullptr;
	}
	return &m_ResMaterials[index];
}

}