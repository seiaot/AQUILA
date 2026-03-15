#include "Fwk/Shape/Shape.h"


using namespace Lib::Graphics;

namespace Fwk {

Shape::Shape()
: m_pVertex(nullptr)
, m_pIndices(nullptr)
, m_VerticesNum(0)
, m_IndicesNum(0)
, m_Topology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED){

}

Shape::~Shape() {
	Term();
}

void Shape::Init(ID3D12Device* pDevice, const ResMesh& resMesh, D3D_PRIMITIVE_TOPOLOGY topology)
{
	if (pDevice == nullptr)
		return;

	m_Topology = topology;

	m_VerticesNum = (int)resMesh.Vertices.size();
	m_IndicesNum = (int)resMesh.Indices.size();

	m_Mesh.Init(pDevice, resMesh);

	m_pVertex = m_Mesh.GetVertexBuffer().Map<MeshVertex>();
	m_pIndices = m_Mesh.GetIndexBuffer().Map();
}

void Shape::Term()
{
	if (m_pVertex) {
		m_Mesh.GetVertexBuffer().Unmap();
		m_pVertex = nullptr;
	}

	if (m_pIndices) {
		m_Mesh.GetIndexBuffer().Unmap();
		m_pIndices = nullptr;
	}

	m_Mesh.Term();
	m_Topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

void Shape::Draw(ID3D12GraphicsCommandList* pCmdList)
{
	if (m_pVertex == nullptr)
		return;

	if (m_Topology == D3D_PRIMITIVE_TOPOLOGY_UNDEFINED)
		return;

	pCmdList->IASetPrimitiveTopology(m_Topology);
	m_Mesh.Draw(pCmdList);
}

void Shape::SetTexCoord(const int index, const XMFLOAT2& coord)
{
	if (m_pVertex == nullptr)
		return;

	if (index < 0 || index >= m_VerticesNum)
		return;

	m_pVertex[index].TexCoord = coord;
}

void Shape::SetTexCoord(int startIndex, const XMFLOAT2 coord[],int coordNum) {

	if (m_pVertex == nullptr)
		return;

	int index = startIndex;

	for (int i = 0; i < coordNum; ++i) {

		if (index >= m_VerticesNum)
			return;

		m_pVertex[index].TexCoord = coord[i];
		++index;
	}
}

void Shape::SetColor(int index, const XMFLOAT4& color)
{
	if (m_pVertex == nullptr)
		return;

	if (index < 0 || index >= m_VerticesNum)
		return;

	m_pVertex[index].Color = color;
}

void Shape::SetColor(int startIndex, const XMFLOAT4 color[], int colorNum)
{
	if (m_pVertex == nullptr)
		return;

	int index = startIndex;

	for (int i = 0; i < colorNum; ++i) {

		if (index >= m_VerticesNum)
			return;

		m_pVertex[index].Color = color[i];
		++index;
	}
}

void Shape::SetColor(int index, const XMFLOAT3& color)
{
	if (m_pVertex == nullptr)
		return;

	if (index < 0 || index >= m_VerticesNum)
		return;

	m_pVertex[index].Color.x = color.x;
	m_pVertex[index].Color.y = color.y;
	m_pVertex[index].Color.z = color.z;
}

void Shape::SetColor(int startIndex, const XMFLOAT3 color[], int colorNum)
{
	if (m_pVertex == nullptr)
		return;

	int index = startIndex;

	for (int i = 0; i < colorNum; ++i) {

		if (index >= m_VerticesNum)
			return;

		m_pVertex[index].Color.x = color[i].x;
		m_pVertex[index].Color.y = color[i].y;
		m_pVertex[index].Color.z = color[i].z;

		++index;
	}
}

void Shape::SetAlpha(int index, float alpha)
{
	if (m_pVertex == nullptr)
		return;

	if (index < 0 || index >= m_VerticesNum)
		return;

	m_pVertex[index].Color.w = alpha;
}

void Shape::SetAlpha(int startIndex, const float alpha[],int alphaNum)
{
	if (m_pVertex == nullptr)
		return;

	int index = startIndex;

	for (int i = 0; i < alphaNum; ++i) {

		if (index >= m_VerticesNum)
			return;

		m_pVertex[index].Color.w = alpha[i];

		++index;
	}
}

void Shape::SetAdditionalColor(int index, const XMFLOAT3& color) {

	if (m_pVertex == nullptr)
		return;

	if (index < 0 || index >= m_VerticesNum)
		return;

	m_pVertex[index].AddColor = color;
}

void Shape::SetAdditionalColor(int startIndex, const XMFLOAT3 color[], int colorNum) {

	if (m_pVertex == nullptr)
		return;

	int index = startIndex;

	for (int i = 0; i < colorNum; ++i) {

		if (index >= m_VerticesNum)
			return;

		m_pVertex[index].AddColor = color[i];

		++index;
	}
}

void Shape::SetVertexPosotion(int index, const XMFLOAT3& position) {
	if (m_pVertex == nullptr)
		return;

	if (index < 0 || index >= m_VerticesNum)
		return;

	m_pVertex[index].Position = position;
}

void Shape::SetIndices(const std::vector<uint32_t>& indices) {
	if (m_pIndices == nullptr)
		return;

	int indexNum = (m_IndicesNum < indices.size()) ? m_IndicesNum : (int)indices.size();

	for(int i = 0; i < indexNum; ++i) {
		m_pIndices[i] = indices[i];
	}

	m_Mesh.SetIndexNum(indexNum);
}

void Shape::SetIndices(const uint32_t indices[], int indexNum) {
	if (m_pIndices == nullptr)
		return;

	indexNum = (m_IndicesNum < indexNum) ? m_IndicesNum : indexNum;

	for (int i = 0; i < indexNum; ++i) {
		m_pIndices[i] = indices[i];
	}
	m_Mesh.SetIndexNum(indexNum);
}


void Shape::SetTopology(D3D_PRIMITIVE_TOPOLOGY topology) {
	m_Topology = topology;
}

}//namespace Fwk