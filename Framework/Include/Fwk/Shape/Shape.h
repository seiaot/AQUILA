#pragma once


#include <d3d12.h>
#include <dxgi.h>

#include "Lib/Graphics/MeshIB.h"

using namespace Lib::Graphics;

namespace Fwk{

class Shape
{
public:
	Shape();
	~Shape();

	void Init(ID3D12Device* pDevice,const ResMesh& resMesh, D3D_PRIMITIVE_TOPOLOGY topology) ;
	void Term() ;
	void Draw(ID3D12GraphicsCommandList* pCmdList) ;

	void SetTexCoord(int index, const XMFLOAT2& coord);
	void SetTexCoord(int startIndex, const XMFLOAT2 coord[], int coordNum);

	void SetColor(int index, const XMFLOAT4& color);
	void SetColor(int startIndex, const XMFLOAT4 color[], int colorNum);

	void SetColor(int index, const XMFLOAT3& color);
	void SetColor(int startIndex, const XMFLOAT3 color[], int colorNum);

	void SetAlpha(int index, float alpha);
	void SetAlpha(int startIndex, const float alpha[],int alphaNum);

	void SetAdditionalColor(int index, const XMFLOAT3& color);
	void SetAdditionalColor(int startIndex, const XMFLOAT3 color[], int colorNum);

	void SetVertexPosotion(int index, const XMFLOAT3& position);
	void SetIndices(const std::vector<uint32_t>& indices);
	void SetIndices(const uint32_t indices[], int indexNum);

	void SetTopology(D3D_PRIMITIVE_TOPOLOGY topology);

private:
	MeshIB m_Mesh;
	MeshVertex* m_pVertex;
	uint32_t* m_pIndices;
	int m_VerticesNum;
	int m_IndicesNum;
	D3D_PRIMITIVE_TOPOLOGY m_Topology;
};

}//namespace Fwk