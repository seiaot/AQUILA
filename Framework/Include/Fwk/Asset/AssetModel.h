// ----------------------------------------------------
// モデルアセットクラス
// 
// 
// 
// ----------------------------------------------------
#pragma once

#include "Asset.h"
#include "Lib/Graphics/TextureSRV.h"
#include "Lib/Graphics/ResMesh.h"
#include "Lib/Graphics/MeshIB.h"

using namespace Lib::Graphics;

namespace Fwk
{
class AssetModel : public Asset
{
	friend class AssetManager;
	friend class AssetHandle;

public:

	uint32_t GetMeshCount()const;
	MeshIB* GetMesh(uint32_t index);

	uint32_t GetMaterialCount()const;
	ResMaterial* GetMaterial(uint32_t index);

protected:

	AssetModel();

	bool Init(const wchar_t* pFilePath) override;

	void OnTerm() override;


private:

	std::vector<ResMesh> m_ResMeshes;
	std::vector<ResMaterial> m_ResMaterials;
	MeshIB* m_pMeshIBs;

	uint32_t        m_MeshNum;
};

}