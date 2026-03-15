#include "Fwk/Shape/ShapePlane.h"

#include "Lib/LibDef.h"

#pragma comment( lib, "d3d12.lib" )
#pragma comment( lib, "dxgi.lib" )

#include "Lib/Graphics/ResMesh.h"

using namespace Lib::Graphics;

namespace Fwk {

ShapePlane::ShapePlane()
	: m_pVertex(nullptr)
{

}

ShapePlane::~ShapePlane()
{
	Term();
}

void ShapePlane::Init(ID3D12Device* pDevice)
{
	if (pDevice == nullptr)
		return;

	const XMFLOAT3 Position[4]={
		{-0.5f, 0.0f, 0.5f},
		{ 0.5f, 0.0f, 0.5f},
		{ 0.5f, 0.0f,-0.5f},
		{-0.5f, 0.0f,-0.5f},
	};
	const XMFLOAT3 Normal = { 0.0f,1.0f,0.0f };
	const XMFLOAT4 Color = { 1.0f,1.0f, 1.0f, 1.0f };
	const XMFLOAT2 TexCoord[4] = {
		{0.0f,0.0f},
		{1.0f,0.0f},
		{1.0f,1.0f},
		{0.0f,1.0f},
	};
	const XMFLOAT3 Tangent = { 1.0f,0.0f,0.0f };
	const XMFLOAT3 AddColor = { 0.0f,0.0f,0.0f };

	const MeshVertex vtx[4] = {
		{Position[0],Normal,Color,TexCoord[0],Tangent,AddColor},
		{Position[1],Normal,Color,TexCoord[1],Tangent,AddColor},
		{Position[2],Normal,Color,TexCoord[2],Tangent,AddColor},
		{Position[3],Normal,Color,TexCoord[3],Tangent,AddColor},
	};
	
	ResMesh resMesh;
	
	resMesh.Vertices.push_back(vtx[0]);
	resMesh.Vertices.push_back(vtx[1]);
	resMesh.Vertices.push_back(vtx[2]);
	resMesh.Vertices.push_back(vtx[3]);

	resMesh.Indices.push_back(0);
	resMesh.Indices.push_back(1);
	resMesh.Indices.push_back(2);

	resMesh.Indices.push_back(0);
	resMesh.Indices.push_back(2);
	resMesh.Indices.push_back(3);

	resMesh.MaterialId = 0;

	m_Mesh.Init(pDevice, resMesh);

	m_pVertex = m_Mesh.GetVertexBuffer().Map<MeshVertex>();
}

void ShapePlane::Term() {

	if (m_pVertex) {
		m_Mesh.GetVertexBuffer().Unmap();
		m_pVertex = nullptr;
	}

	m_Mesh.Term();
}

void ShapePlane::Draw(ID3D12GraphicsCommandList* pCmdList){
	m_Mesh.Draw(pCmdList);
}

void ShapePlane::SetTexCoord(const XMFLOAT2& left_top, const XMFLOAT2& right_bottom) 
{
	if (m_pVertex == nullptr)
		return;

	m_pVertex[0].TexCoord.x = left_top.x;
	m_pVertex[0].TexCoord.y = left_top.y;

	m_pVertex[1].TexCoord.x = right_bottom.x;
	m_pVertex[1].TexCoord.y = left_top.y;

	m_pVertex[2].TexCoord.x = right_bottom.x;
	m_pVertex[2].TexCoord.y = right_bottom.y;

	m_pVertex[3].TexCoord.x = left_top.x;
	m_pVertex[3].TexCoord.y = right_bottom.y;
}

void ShapePlane::SetColor(float r, float g, float b, float a) {
	if (m_pVertex == nullptr)
		return;

	for (int i = 0; i < 4; ++i) {
		m_pVertex[i].Color.x = r;
		m_pVertex[i].Color.y = g;
		m_pVertex[i].Color.z = b;
		m_pVertex[i].Color.w = a;
	}
}

void ShapePlane::SetColor(float r, float g, float b) {
	if (m_pVertex == nullptr)
		return;

	for (int i = 0; i < 4; ++i) {
		m_pVertex[i].Color.x = r;
		m_pVertex[i].Color.y = g;
		m_pVertex[i].Color.z = b;
	}
}

void ShapePlane::SetAlpha(float a) {
	if (m_pVertex == nullptr)
		return;

	for (int i = 0; i < 4; ++i) {
		m_pVertex[i].Color.w = a;
	}
}

}//namespace Fwk