#include "Fwk/Graphics/LineSegments.h"

#include <DirectXMath.h>
#include "Lib/Util/Log.h"
#include "Fwk/Framework.h"

using namespace Lib::Graphics;

LineSegments::LineSegments()
	: m_RotDeg()
	, m_RotMtx(DirectX::XMMatrixIdentity())
	, m_TransMtx(DirectX::XMMatrixIdentity())
	, m_ScaleMtx(DirectX::XMMatrixIdentity())
	, m_isFlipX(false)
	, m_isFlipY(false)
	, m_IsInitialized(false)
	, m_vertexMax(0)
{
	;
}

void LineSegments::Init(int vtxNum, int indexNum)
{
	//既に初期化済みであれば警告を出したうえで再初期化
	if (m_IsInitialized) {
		Debug::Logf("初期化済みのLineSegmentsを再初期化しました。\n");
		Term();
	}

	//定数バッファの生成
	for (auto i = 0u; i < Graphics::FrameCount; ++i) {
		RenderManager_I->CreateConstantBuffer(m_ConstantBuffer[i]);
	}

	m_isFlipX = false;
	m_isFlipY = false;

	{
		MeshVertex vtx;
		vtx.Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vtx.Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vtx.Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vtx.TexCoord = XMFLOAT2(0.0f, 0.0f);
		vtx.Tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
		vtx.AddColor = XMFLOAT3(0.0f, 0.0f, 0.0f);

		ResMesh resMesh;

		for (int i = 0; i < vtxNum; ++i) {
			resMesh.Vertices.push_back(vtx);
		}
		for (int i = 0; i < indexNum; ++i) {
			resMesh.Indices.push_back(i);
		}

		resMesh.MaterialId = 0;

		m_shape.Init(RenderManager_I->GetGraphics()->GetDevice(), resMesh, D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}

	m_vertexMax = vtxNum;

	m_IsInitialized = true;
}

LineSegments::~LineSegments()
{
	Term();

}

void LineSegments::Term()
{
	if (!m_IsInitialized) {
		return;
	}

	//Frameworkが破棄されていたら何もしない
	auto fwk = Framework_I;
	if (fwk == nullptr)
		return;

	for (auto i = 0u; i < Graphics::FrameCount; ++i) {
		m_ConstantBuffer[i].Term();
	}
	m_shape.Term();
	m_IsInitialized = false;
}

void LineSegments::Update()
{
	;
}

void LineSegments::Draw()
{
	RenderManager_I->RequestRender(this);
}

void LineSegments::SetRotation(float rotDeg) {
	m_RotDeg[2] = rotDeg;
	m_RotMtx = XMMatrixRotationZ(DirectX::XMConvertToRadians(rotDeg));
}

//回転（全軸対象）　引数はDegree 0.0f～360.0fで一周
void LineSegments::SetRotation(float rotDegX, float rotDegY, float rotDegZ)
{
	m_RotDeg[0] = rotDegX;
	m_RotDeg[1] = rotDegY;
	m_RotDeg[2] = rotDegZ;
	// Rotates about y-axis (Yaw), then x-axis (Pitch), then z-axis (Roll)
	m_RotMtx = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(rotDegX)
		, XMConvertToRadians(rotDegY)
		, XMConvertToRadians(rotDegZ)
	);
}

//ベクトルの向きに回転する
void LineSegments::SetRotation(const Vector2f& vDirection) {

	const Vector2f right(1.0f, 0.0f);

	Vector2f vDir = vDirection.GetNormalized();

	float cos = Vector2f::Dot(right, vDir);
	float sin = Vector2f::Cross(right, vDir);
	float rad = acosf(cos);

	if (sin < 0.0f) {
		rad = XM_2PI - rad;
	}
	SetRotation(XMConvertToDegrees(rad));
}

void LineSegments::SetPosition(float x, float y){
	m_TransMtx = XMMatrixTranslation(x, y, 0.0f);
}

void LineSegments::SetPosition(const Lib::Math::Vector2f& vPos) {
	m_TransMtx = XMMatrixTranslation(vPos.x, vPos.y, 0.0f);
}

//頂点座標設定(2D) SetPositionした位置からの相対位置
void LineSegments::SetVertexPosition(int vtxIndex, float x, float y)
{
	m_shape.SetVertexPosotion(vtxIndex, XMFLOAT3(x, y, 0.0f));
}

//頂点座標設定(2D) SetPositionした位置からの相対位置
void LineSegments::SetVertexPosition(int vtxIndex, const Vector2f& vPos)
{
	m_shape.SetVertexPosotion(vtxIndex, XMFLOAT3(vPos.x, vPos.y, 0.0f));
}

//頂点座標設定(2D) SetPositionした位置からの相対位置
void LineSegments::SetVertexPosition(int vtxIndex, const XMFLOAT3& vPos) {
	m_shape.SetVertexPosotion(vtxIndex, vPos);
}

//インデックス設定
void LineSegments::SetIndices(const uint32_t indices[], int indexNum)
{
	m_shape.SetIndices(indices,indexNum);
}

//インデックス設定
void LineSegments::SetIndices(const std::vector<uint32_t>& indices)
{
	m_shape.SetIndices(indices);
}

void LineSegments::SetScale(float sx, float sy) {
	m_ScaleMtx = XMMatrixScaling(sx, sy, 1.0f);
}
void LineSegments::SetScale(const Vector2f& vScale) {
	SetScale(vScale.x, vScale.y);
}

//パラメータ取得系
float LineSegments::GetRotationX()const {
	return m_RotDeg[0];
}
float LineSegments::GetRotationY()const {
	return m_RotDeg[1];
}
float LineSegments::GetRotationZ()const {
	return m_RotDeg[2];
}
float LineSegments::GetPositionX()const {
	return m_TransMtx.r[3].m128_f32[0];
}
float LineSegments::GetPositionY()const{
	return m_TransMtx.r[3].m128_f32[1];
}
Vector2f LineSegments::GetPosition()const{
	Lib::Math::Vector2f v(
		m_TransMtx.r[3].m128_f32[0],
		m_TransMtx.r[3].m128_f32[1]
	);
	return v;
}

float LineSegments::GetScaleX()const {
	return m_ScaleMtx.r[0].m128_f32[0];
}
float LineSegments::GetScaleY()const {
	return m_ScaleMtx.r[1].m128_f32[1];
}

void LineSegments::OnDraw(IGraphics* g,Camera* pCamera)
{
	//既に初期化済みであれば警告を出したうえで再初期化
	if (!m_IsInitialized) {
		Debug::Logf("未初期化のLineSegmentsの描画を試みました。\n");
		return;
	}

	auto pCommandList = g->GetCommandList();
	auto frameIndex = g->GetFrameIndex();

	//この第一引数のINDEXはルートシグネチャのパラメータと対応している必要があるよ
	pCommandList->SetGraphicsRootConstantBufferView(0, m_ConstantBuffer[frameIndex].GetAddress());

	//ワールド行列の設定
	Fwk::CB_Transform* pTrans = (Fwk::CB_Transform*)m_ConstantBuffer[frameIndex].GetPtr();

	DirectX::XMMATRIX mtxFlip = DirectX::XMMatrixIdentity();
	if (m_isFlipX) {
		mtxFlip.r[0].m128_f32[0] = -1.0f;
	}
	if (m_isFlipY) {
		mtxFlip.r[1].m128_f32[1] = -1.0f;
	}

	pTrans->World = (m_ScaleMtx ) * m_RotMtx * mtxFlip * (m_TransMtx);
	pTrans->View = pCamera->GetViewMatrix();
	pTrans->Proj = pCamera->GetProjectionMatrix();

	m_shape.Draw(pCommandList->Get());
}

RENDER_PASS LineSegments::GetRenderPass()const {
	return RENDER_PASS::Sprite;
}

RENDER_MODE LineSegments::GetRenderMode()const
{
	return RENDER_MODE::LineSegments;
}

void LineSegments::SetColor(float r, float g, float b) {
	for (int i = 0; i < m_vertexMax; ++i) {
		m_shape.SetColor(i,XMFLOAT3(r, g, b));
	}
}

void LineSegments::SetColor(int vertexIndex,float r, float g, float b) {
	m_shape.SetColor(vertexIndex, XMFLOAT3(r, g, b));
}

//X軸フリップ
void LineSegments::SetFlipX(bool isFlip) {
	m_isFlipX = isFlip;
}
bool LineSegments::IsFlipX()const {
	return m_isFlipX;
}

//Y軸フリップ
void LineSegments::SetFlipY(bool isFlip) {
	m_isFlipY = isFlip;
}
bool LineSegments::IsFlipY()const {
	return m_isFlipY;
}

void LineSegments::SetTopology(D3D_PRIMITIVE_TOPOLOGY topology) {
	m_shape.SetTopology(topology);
}
